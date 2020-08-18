#!/bin/bash

CONFIG="$(tr [a-z] [A-Z] <<< "$1")"

if [ "$CONFIG" == "RELEASE" ]; then
    $2 $3 $4 $5 $6 $7 $8 $9
fi
