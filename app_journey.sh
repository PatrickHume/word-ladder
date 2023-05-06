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
# Take array and return items delimited with " and "
function echo_array_with_delimiter {
  local arr=("$@")  # store the array as a local variable
  local last_index=$((${#arr[@]} - 1))  # calculate the index of the last element

  # loop through each element of the array
  for i in $(seq 0 $last_index); do
    echo -n "${arr[$i]}"  # echo the current element
    if [[ $i -lt $last_index ]]; then
      echo -n " and "  # if this is not the last element, add the delimiter
    fi
  done
  echo ""  # add a newline at the end
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
echo -n "Showing shortest word ladder between "
echo_array_with_delimiter "${words[@]^^}"
echo "$result"
read -n1 -p "Press any key to continue... "
clear
