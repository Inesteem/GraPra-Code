#!/bin/bash
xterm -geometry 200x100+0+900 -e "./src/multiman_server 1 lvl02 $*; echo '-- '; echo 'press return to close this terminal'; read"    &
sleep 1
./src/multiman localhost $*
