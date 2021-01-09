# Simple-TagoIO-Example

A minimal example of an ESP8266 based microcontroller sketch sending "fake" measurement values to a bucket at https://tago.io.

This repository contains a PlatformIO project folder. For instructions on how to use in Arduino IDE see below.

Corresponding blog entry on https://net-things.de/index.php/blog/verarbeitung-von-sensordaten-mit-tagoio (though my blog is in german language only).

### Configuration

Set your WiFi access point credentials and the device token of your device at TagoIO in `platformio.ini`:
```build_flags =
-DSTASSID=\"my-ssid\"
-DSTAPSK=\"my-password\"
-DTAGO_DEVICE_TOKEN=\"1234567-1234-1234-1234-123456789\"
```

A NodeMCU board is used in the example. If using a different microcontroller, adjust the `board` settings in `platformio.ini`.

### Instructions for use with Arduino IDE

- rename `main.cpp` to `Simple-TagoIO-Example.ino`
- add the following lines at the top of `Simple-TagoIO-Example.ino`:
   ``` 
   #define STASSID "your-wifi-ssid"
   #define STAPSK "your-wifi-password"
   #define TAGO_DEVICE_TOKEN "your-device-token"
   ```
- remove `main.h` and `platformio.ini`, it's not needed on Arduino
- choose your correct board and settings in the board manager
