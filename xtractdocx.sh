#!/bin/bash
# a small script for extracting the text and the media from a docx file
# destroys layout (
# tested on cygwin, ensure that unzip is installed
# more: http://wp.me/p4fGy5-60
# usage: $ ./xtractdocx.sh file.docx

foldername=$1$(date +%s)
mkdir $foldername
mkdir $foldername/media

echo "* extracting media files to ./$foldername/media"
unzip $1 'word/media/*' -d ./$foldername
mv ./$foldername/word/media/* ./$foldername/media
echo "* ls ./$foldername/media"
ls ./$foldername/media

echo "* extracting xml content"
unzip $1 'word/document.xml' -d ./$foldername
echo "* write xml to txt content to ./$foldername/document.txt"
sed -e 's/<[^>]*>//g' ./$foldername/word/document.xml > ./$foldername/document.txt
echo "* cleanup"
rm -rf ./$foldername/word

echo "* done"
