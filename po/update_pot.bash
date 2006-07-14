#!/bin/bash
xgettext --keyword=_ --debug --from-code=en -C -D .. -p . -f POTFILES.in -o avidemux.pot
echo "DONE."
