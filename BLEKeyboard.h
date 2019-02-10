#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include "HIDKeyboardTypes.h"
#include <driver/adc.h>
#include <Arduino.h>


static const char* TAG = "BTEKey";

extern bool connected;
static BLEHIDDevice* hid;
static BLECharacteristic* input;
static BLECharacteristic* output;
void taskServer(void*);
void sendMyKey(KEYMAP map);
void BTSend(int keycode);
