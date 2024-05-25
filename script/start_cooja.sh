#!/bin/bash

# Move to the directory where the script is located
destination="$HOME/contiki-ng/tools/cooja/"

# Check if the destination folder exists
if [ -d "$destination" ]; then
    echo "Move to the destination folder"
    # Move to the destination folder
    cd "$destination" || exit 1  # Exit if change directory fails
    # Execute the command in the destination folder
    echo "Execute the command in the destination folder"
    
    command="./gradlew run"
    $command
else
    echo "The destination folder does not exist"
    exit 1
fi
