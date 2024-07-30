#!/bin/sh

## Max 3000 lines 

# File to store kernel messages
LOGFILE="/mnt/flash/external/kernel.log"
TMPFILE="/tmp/dmesg.tmp"
PREVFILE="/tmp/dmesg.prev"

# Ensure the log file exists
if [ ! -e "$LOGFILE" ]; then
    touch "$LOGFILE"
fi

# Get the current kernel messages
dmesg > "$TMPFILE"

# If previous log file exists, extract new lines and append them
if [ -e "$PREVFILE" ]; then
    comm -13 "$PREVFILE" "$TMPFILE" >> "$LOGFILE"
else
    # If no previous file exists, append all current messages
    cat "$TMPFILE" >> "$LOGFILE"
fi

# Update the previous log file
cp "$TMPFILE" "$PREVFILE"

# Trim the logfile to the most recent MAX_LINES lines
MAX_LINES=3000
LINE_COUNT=$(wc -l < "$LOGFILE")

if [ "$LINE_COUNT" -gt "$MAX_LINES" ]; then
    tail -n "$MAX_LINES" "$LOGFILE" > "$LOGFILE.tmp" && mv "$LOGFILE.tmp" "$LOGFILE"
fi

