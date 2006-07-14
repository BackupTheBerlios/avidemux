#!/bin/bash
xgettext --keyword=_ --debug --from-code=utf-8 -C -D .. -p . -f POTFILES.in -o avidemux.pot
echo "DONE."
