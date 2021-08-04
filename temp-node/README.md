# Temperature and Humidity Node
This is a very simple sensor node that reports humidity and temperature. It is designed to be very cheap and USB-powered, so these can be sprinkled around the home. Each sensor node is an ESP8266 NodeMCU with a [DH11 Temperature Sensor](https://www.amazon.com/gp/product/B01DKC2GQ0/) wired to it.

## Hardware Setup
The DHT11 wiring is very simple:
- DHT11 `+` pin to the ESP8266 `VIN/5V` pin
- DHT11 `-` pin to an ESP8266 `GND` pin
- DHT11 `out` pin to ESP8266 `GPIO14` (marked `D5`)


## Software Setup
Rename `arduino_secrets.h.example` to `arduino_secrets.h` and change the wifi SSID and password in it to match the network you wish to attach to.

You'll need to have the Arduino-cli toolchain and gnu make installed. Once those prerequisites are satisfied, connect the ESP8266 to your computer, enter a command line in the `sensor-node` directory and run:
```
make config-tools
make program
```
Note that you may have to edit the serial port defined at the top of the Makefile.

Once programming is complete, open a web browser to [http://unnamed.local/](http://unnamed.local/). This will show you a simple status page verifying that the temperature and humidity readings are working. Additionally, it allows you to rebrand the sensor node with a more useful name (e.g. "bedroom", "upstairs", etc.). This name will be used for the URL and MQTT topic, so it can't have any spaces or other weird characters in it.

Repeat these steps with as many sensors as you like, and they'll start publishing temperature and humidity data on your network.
