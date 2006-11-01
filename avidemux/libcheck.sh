#!/bin/bash
echo "Checking malloc..."
find . -name "*.a" | xargs nm -AC | grep "malloc\|free"| grep   " U " | grep -v ".* U .*_.*"
echo "Checking fread/fwrite..."
find . -name "*.a" | xargs nm -AC | grep "fread\|fwrite"| grep   " U " | grep -v ".* U .*_.*"
echo "Checking GTK..."
find . -name "*.a" | grep -vi gtk | xargs nm -AC | grep "gtk_"| grep   " U " 
