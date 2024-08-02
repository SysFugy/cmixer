#!/bin/bash

sink=$(pactl info | grep "Default Sink:" | awk '{print $3}')

if [ -z "$sink" ]; then
    echo "Null"
    exit 1
fi

volume=$(pactl get-sink-volume "$sink")
percent=$(echo "$volume" | awk '{print $5}')

echo $percent
