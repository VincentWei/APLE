#!/usr/bin/env bash
#
# Here is a simple bash script that iterates all directories and files
# recursively to calculate the total number of directories and files.
#
dirs=0
files=0

function scan_dir() {
	all=$(ls -a "$1")
	# for file in "$1"/*; do
	for file in $(ls -a "$1"); do
		if [ "$file" == "." ]; then
			continue
		elif [ "$file" == ".." ]; then
			continue
		fi

		fullname="$1/$file"
		if [ -d "$fullname" ]; then
			echo "DIR: $fullname"
			((dirs++))
			scan_dir "$fullname"
		elif [ -f "$fullname" ]; then
			echo "FILE: $fullname"
			((files++))
		fi
	done
}

while [ "$1" != '' ]; do
	scan_dir "$1"
	shift
done

echo "Directories: $dirs"
echo "Files: $files"

# This script takes a list of directories as arguments, and for each directory
# passed as an argument, it recursively scans for other directories and files.
# It increments the `dirs` (directories) and `files` (files) counters every
# time it finds a directory or a file respectively.
#
# To use this script, save it to a file, let's say, `total_files.sh`, give it
# execute permissions (`chmod +x total_files.sh`) and then run it like
# `./total_files.sh /path/to/directory`. `/path/to/directory` will be the
# directory you want to count the total number of files and directories inside
# it.
#
# Please note that this script might not work as expected if you have nested
# symbolic links. For a more accurate count, you should implement an additional
# file type checking, and handle special cases as necessary, in your case
# specific to your business requirements.
