#!/bin/bash
xgettext --debug -a --from-code=en -C -D .. -p . -f POTFILES.in -o avidemux2.pot
echo "DONE."
