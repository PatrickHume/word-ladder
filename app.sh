#!/bin/bash
# Define menu options and state transitions
options=("Connect words" "Word neighbours" "Aloof words" "Longest ladder")
transitions=(1 2 3 4 q)
invalid_input=false
function longest_path() {
    clear
    echo "Note that this program takes a while to run."
    read -n1 -p "Press 'r' to return or press any key to continue... " input && [ "$input" = "r" ] && return
    ./lspath 
    echo "Press any key to continue..."
    read
}
# Loop until the user chooses to quit
while true; do
    clear
    echo "Word Ladder App"
    echo "---------------"
    for i in "${!options[@]}"; do
        echo "$((i+1)). ${options[$i]}"
    done
    if $invalid_input 
    then
        echo "Invalid option."
    fi
    read -p "Choose a number or enter q to quit: " choice
    if [[ " ${transitions[@]} " =~ " $choice " ]]; then
        case $choice in
            1)  
                bash app_journey.sh ;;
            2)  
                bash app_neighbours.sh ;;
            3)  
                clear
                ./aloof | tr '[:lower:]' '[:upper:]'
                read -n1 -p "Press any key to continue... " ;;
            4)  
                longest_path ;;
            q)  
                clear 
                break;;
        esac
        invalid_input=false
    else
        invalid_input=true
    fi
done
