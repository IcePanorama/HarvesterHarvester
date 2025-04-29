## Create an md5 checksum for every file in a given directory.
#! /usr/bin/env bash
directory="$1"
output_file="checksums.txt"

if [ -z "$directory" ]; then
    echo "Please provide a directory path."
    exit 1
fi

if [ ! -d "$directory" ]; then
    echo "The provided directory does not exist."
    exit 1
fi

> "$output_file"

find "$directory" -type f | while read file; do
    md5sum "$file" >> "$output_file"
done

echo "MD5 checksums have been written to $output_file."

