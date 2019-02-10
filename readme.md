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

## resources
* BLE keybode example code [sabas1080/ESP32_HID.ino](https://gist.github.com/sabas1080/93115fb66e09c9b40e5857a19f3e7787)
* other bluetooth hid example https://github.com/MagicCube/esp32-keynotes-controller/
* makefile https://raw.githubusercontent.com/plerup/makeEspArduino/
