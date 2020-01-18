#!/bin/bash
echo Running..
xterm -geometry 60x25+10+0 -e ./server &
xterm -geometry 60x25+1000+10 -e ./client1 32002 Hello_from_first_client &
xterm -geometry 60x25+1000+1000 -e ./client2 32014 &
xterm -geometry 60x25+500+10 -e ./client1 32003 Hello_from_second_client &
xterm -geometry 60x25+500+1000 -e ./client2 32015
