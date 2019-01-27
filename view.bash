#!/usr/bin/env bash
stty -F /dev/ttyUSB* cs8 115200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts 
tail -f < /dev/ttyUSB* 
