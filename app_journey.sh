#!/bin/bash
# Take number and return ordinal e.g ordinal 1 -> 1st
function ordinal () {
  case "$1" in
    *1[0-9] | *[04-9]) echo "$1"th;;
    *1) echo "$1"st;;
    *2) echo "$1"nd;;
    *3) echo "$1"rd;;
  esac
}

# Join array elements by string
function join_by {
  local d=${1-} f=${2-}
  if shift 2; then
    printf %s "$f" "${@/#/$d}"
  fi
}

# Get two words
invalid_input=false
words=()
for (( i=1; i<=2; i++ ))
do
    valid_word=false
    while [ $valid_word == false ]
    do
        clear
        # Notify user if last input was invalid
        if $invalid_input
        then
            echo "Invalid word. Please enter a 5-letter word."
        fi
        # Prompt for word
        read -p "Enter $(ordinal $i) 5-letter word or type r to return: " choice
        # Check if exiting program
        if [ "$choice" = "r" ]; then
            state=0
            exit
        fi
        # Check if length 5
        if [ ${#choice} -eq 5 ]
        then
            # Append lowercase word to array
            valid_word=true
            words+=(${choice,,})
            invalid_input=false
        else
            # Record input as invalid and try again
            invalid_input=true
        fi
    done
done
# Call app script with lowercase words as arguments and convert output to uppercase
result=$(./journey $(printf ' %s' "${words[@]}") | tr '[:lower:]' '[:upper:]')
# Output results
clear
echo "Showing shortest word ladder between $(join_by ' and ' ${words[@]^^})"
echo "$result"
read -n1 -p "Press any key to continue... "
clear
