#!/bin/bash
xterm -e "./src/multiman_server 1 $*; echo '-- '; echo 'press return to close this terminal'; read"    &
sleep 1
./src/multiman localhost $*
