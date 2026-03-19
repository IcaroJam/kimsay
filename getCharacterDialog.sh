#!/bin/bash

# Find, sanitize and format the dialog from the character
# with code $1 and save it as $2.json
# E.g.: ./getCharacterDialog.sh 397 conceptualization

IN=wholeDialog.json
OUT=dialog/$2.json

echo -e '{\n\t"txt": [' > $OUT # Create the opening lines of the json
grep -e "$1, \"" $IN | sed -r 's/\s+\[([0-9]+, ){3}(".*")$/\t\t\2,/' | sort -u >> $OUT # Remove array stuff and indent
sed -i '$s/,$//' $OUT # Remove trailing comma of last line
echo -e '\t]\n}' >> $OUT # Append closing lines of json