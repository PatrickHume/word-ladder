#!/bin/bash
invalid_input=false
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
    read -p "Enter a 5-letter word or type r to return: " choice
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
        invalid_input=false
    else
        # Record input as invalid and try again
        invalid_input=true
    fi
done

# Call app script with lowercase words as arguments and convert output to uppercase
result=$(./neighbours "${choice}" | tr '[:lower:]' '[:upper:]')
# Output results
clear
echo "Showing neigbours of ${choice^^}"
echo "$result"
read -n1 -p "Press any key to continue... "
clear
