#!/bin/bash
echo "Checking Deps..."
find . -name "*.a" | xargs nm -AC | grep "fread\|fwrite\|malloc"| grep   " U " | grep -v ".* U .*_.*"
echo "Checking GTK..."
find . -name "*.a" | grep -vi gtk | xargs nm -AC | grep "gtk_"| grep   " U " 
