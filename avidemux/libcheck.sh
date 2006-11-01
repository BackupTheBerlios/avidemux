#!/bin/bash
find . -name "*.a" | xargs nm -AC | grep "gtk_\|fread\|fwrite\|malloc"| grep   " U " | grep -v ".* U .*_.*"
