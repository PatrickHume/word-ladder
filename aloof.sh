#!/bin/bash
# record number of aloof words
counter=0
# iterate over file
while read word; do
    # get first char of output from neighbours
    num_neighbours=$(printf %.1s "$(./neighbours $word)")
    # record words with 0 neighbours
    if [[ $num_neighbours -eq 0 ]]; then
        echo $word
        ((counter++))
    fi
# loop over allWords
done <allWords.txt
# display number of aloof words
echo "$counter aloof words"