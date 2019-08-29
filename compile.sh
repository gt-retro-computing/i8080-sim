#!/bin/sh

f=$(mktemp)

node ../8080-assembler/combined.js "$1" | tee "$f"
if [ ! $? -eq 0 ]; then
    echo "Compilation failed"
    exit 1
fi

mv "$f" "$1.hex"
