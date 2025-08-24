#!/bin/sh

awk -F , ' 
/^SW/ { print $1 "," $2 "," $3 "," $4 "," $5+5.03 "," $6 ",bottom"; next }
{print $0; }
' 
