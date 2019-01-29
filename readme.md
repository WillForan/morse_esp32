# Morse Code Bluetooth keyboard with Arduino and ESP32

## get arduino-esp32 and BLE lib 
```bash
git clone https://github.com/espressif/arduino-esp32.git ~/src/lib/esp32
cd ~/src/lib/esp32/tools
./get.py  
 git submodule update --init
cd - 

# make file is from makeEspArduino, but already tracked
# curl 'https://raw.githubusercontent.com/plerup/makeEspArduino/master/makeEspArduino.mk' > Makefile
```


## usage
* normal morse code
* func + double press = shift
* func + func = repeat
* func + morse => symbols and parens
