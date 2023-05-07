#!/bin/bash
# Array of required executables
executables=("aloof" "journey" "lspath" "lspath_opt" "neighbours")

# Check if each executable exists
missing=()
for file in "${executables[@]}"; do
    if ! [ -x "$file" ]; then
        missing+=("$file")
    fi
done

# Join array elements by string
function join_by {
  local d=${1-} f=${2-}
  if shift 2; then
    printf %s "$f" "${@/#/$d}"
  fi
}

# Report any missing files
if [ ! ${#missing[@]} -eq 0 ]; then
    echo "The following executables are missing: $(join_by ', ' ${missing[*]})"
    read -p "Do you want to run make to build them? (y/n) " choice
    if [ "$choice" == "y" ]; then
        clear
        make
        # Check if make succeeded in creating all required executables
        missing=()
        for file in "${executables[@]}"; do
        if ! [ -x "$file" ]; then
            missing+=("$file")
        fi
        done
        if [ ${#missing[@]} -eq 0 ]; then
        echo "All required executables were successfully built!"
        read -n1 -p "Press any key to continue... "
        else
        echo "Failed to build the following executables: $(join_by ', ' ${missing[*]})"
        exit
        fi
    else
        echo "Exiting app..."
        exit
    fi
fi

# Define menu options and state transitions
options=("Connect words" "Word neighbours" "Aloof words" "Longest ladder")
transitions=(1 2 3 4 q)
invalid_input=false
selected=0
clear
# Loop until the user chooses to quit
while true; do
    tput cup 0 0

    echo "Word Ladder App"
    echo "---------------"
    for i in "${!options[@]}"; do
        if [ $i -eq $selected ]; then
            tput setaf 202 # set foreground color to bright orange
            printf "%s\n" "${options[$i]} <"
            tput sgr0 # reset terminal color
        else
            printf "%s\n" "${options[$i]}  "
        fi
    done
    if $invalid_input; then
        printf "Invalid option."
    fi
    
    read -rsn3 input
    case "$input" in
        $'\x1b[A') # up arrow
            ((selected--))
            if [ $selected -lt 0 ]; then
                selected=$((${#options[@]}-1))
            fi
            ;;
        $'\x1b[B') # down arrow
            ((selected++))
            if [ $selected -ge ${#options[@]} ]; then
                selected=0
            fi
            ;;
        $'') # enter key
            choice=$((${transitions[$selected]}))
            if [ $choice -eq $choice ] 2>/dev/null; then
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
                        clear
                        ./lspath_opt | tr '[:lower:]' '[:upper:]'
                        read -n1 -p "Press any key to continue... " ;;
                    q)  
                        clear 
                        break;;
                esac
                clear
                invalid_input=false
            else
                invalid_input=true
            fi
            ;;
        *)
            invalid_input=true
            ;;
    esac
done