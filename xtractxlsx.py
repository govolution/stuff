# PoC code for extracting xlsx files by Daniel Sauder
# https://github.com/govolution/stuff
# License: http://creativecommons.org/licenses/by-sa/3.0

import xml.etree.cElementTree as etree
import os
import sys
import time
import zipfile
import shutil
import codecs
import glob

print "* unpack "+sys.argv[1]+" to tmp directory"
timestamp = int(time.time())
zf=zipfile.ZipFile(sys.argv[1])
tmpdir="./"+sys.argv[1]+str(timestamp)+"tmp"
os.mkdir(tmpdir)
zf.extractall(tmpdir)

workdir="./"+sys.argv[1]+str(timestamp)
os.mkdir(workdir)

sharedStrings=list()
fname1=tmpdir+"/xl/sharedStrings.xml"
print "* read shared strings"
with open(fname1) as xml_file:
	tree = etree.iterparse(xml_file)
	for item in tree:
		if item[1].tag == "{http://schemas.openxmlformats.org/spreadsheetml/2006/main}t":
			sharedStrings.append(item[1].text)		

	
sheetsdir=tmpdir+"/xl/worksheets/"
print sheetsdir
#for sheet in os.listdir(sheetsdir):
for sheet in glob.glob(sheetsdir+"*.xml"):
	#print "sheet "+sheet
	#print "sheet2 "+os.path.basename(sheet)
	sheet=os.path.basename(sheet)
	print "* write " + workdir+"/"+sheet+".txt"
	#fd=open(workdir+"/"+sheet+".txt","w")
	fd=codecs.open(workdir+"/"+sheet+".txt","w","utf-8")
	with open(sheetsdir+sheet) as xml_file:
		tree = etree.iterparse(xml_file)
		line=""
		istext=False
		lastvalue=""
		for item in tree:
			#add last value to line, if string lookup
			if item[1].attrib.get('t')=='s':
				i=int(lastvalue)
				if line is None:
					line=" "
				if sharedStrings[i] is None:
					sharedStrings[i]=" "
				line = line+sharedStrings[i]+";"
			if lastvalue is None:
				line = line+" ;"
			elif len(lastvalue)>0:
				line = line+lastvalue+";"
				
			#get value
			if item[1].tag=='{http://schemas.openxmlformats.org/spreadsheetml/2006/main}v':
				lastvalue=item[1].text
			else:
				lastvalue=""
				
			#print line	
			if item[1].tag=="{http://schemas.openxmlformats.org/spreadsheetml/2006/main}row":
				fd.write(line+"\n")
				line=""

		fd.close()
		
print "* delete tmp directory"
shutil.rmtree(tmpdir)
