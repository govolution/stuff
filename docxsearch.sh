# quick hack: search the content of docx files in a directory recursive
# tested with cygwin, ensure you have unzip installed
# ./docxsearch.sh . "my content"

IFS=$'\n'
for i in $(find $1 | grep docx); do
                {
                               test=$(unzip -p $i word/document.xml | grep -o $2 | sort -u)
                } 2> /dev/null
                size=${#test}
                if [ "$size" -gt "0" ]
                then
                               echo "$i"
                fi
done
unset IFS
