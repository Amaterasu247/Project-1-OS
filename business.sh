####################################
# 1093496 Vasileios Sideris
# 1093306 Konstantinos Alexopoulos
####################################

#!/bin/bash

file_path="Businesses.csv"

function read_business_file() 
{
    read -p "Give the path of the file (press Enter for the default file Businesses.csv): " user_path

    if [ -n "$user_path" ]; then
        file_path="$user_path"
    fi

    if [ -f "$file_path" ]; then
        echo "File $file_path was not found."
    else
        echo "File $file_path was not found. Please check the path and try again."
    fi
}

function view_business_details() 
{
    read -p "Give tha code of a business:" business_code

    awk -F',' -v code="$business_code" '$1 == code {print}' "$file_path"
}

function modify_business_details() 
{
    read -p "Give the code of the business that you wish to change:" business_code

    # Εμφανίζουμε τα αρχικά στοιχεία
    awk -F',' -v code="$business_code" '$1 == code {print}' "$file_path"

    # Επέλεξε στοιχείο προς αντικατάσταση
    read -p "Chose an element from the above record to be replaced:" -r replaced_item

    # Διάλεξε με τι θα αντικαταταστήσει το προς αντικατάσταση στοιχείο
    read -p "Type the value that will replace the element to be replaced:" new_value

    # Χρησιμοποιούμε το sed για την αλλαγή ενός συγκεκριμένου στοιχείου στη γραμμή
    sed -i "/^$business_code/s/$replaced_item/$new_value/" "$file_path"

    # Εμφανίζουμε τα νέα στοιχεία
    awk -F',' -v code="$business_code" '$1 == code {print}' "$file_path"
}

function view_file() 
{
    # Χρησιμοποιούμε το more για σελιδοποίηση
    more "$file_path"
}

function save_file() 
{
    read -p "Give the path of the file in which the store will happen (press Enter for the default file clients.csv): " save_path

    if [ -n "$save_path" ]; then
        file_to_save="$save_path"
    else
        file_to_save="clients.csv"
    fi

    # Χρησιμοποιούμε το sed για να αντικαταστήσουμε μια λέξη με κενό
    sed 's/some_pattern/ /' "$file_path" > "$file_to_save"

    echo "File stored successfully. $file_to_save"
}

while true; 
do
    echo "[1] Select business file"
    echo "[2] View business details"
    echo "[3] Change business item"
    echo "[4] View the files of all the businesses"
    echo "[5] Save file"
    echo "[6] Exit"

    read -p "Chose:" choice

    case $choice in
        1)
            read_business_file
            ;;
        2)
            view_business_details
            ;;
        3)
            modify_business_details
            ;;
        4)
            view_file
            ;;
        5)
            save_file
            ;;
        6)
            echo "Exit. Goodbye!"
            break
            ;;
        *)
            echo "Invalid choice. Please try again."
            ;;
    esac
done