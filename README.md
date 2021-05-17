# temp-network
Network of sensors to monitor temperature and humidity on a local network.

The temperature sensors are a set of ESP8266 nodes with HT11 temperature sensors. These publish temperature and humidity data to a central MQTT server, which then logs the data and serves a webpage with realtime and historical data.

The MQTT server / webserver can be any computer, but I'm using a Raspberry Pi. I have it set up running an avahi mdns daemon to give it a local address at `http://<pi-hostname>.local`.

# Setup

## Sensor Node
Each sensor node is an [ESP8266 NodeMCU](https://www.amazon.com/HiLetgo-Internet-Development-Wireless-Micropython/dp/B010O1G1ES) with a [DH11 Temperature Sensor](https://www.amazon.com/gp/product/B01DKC2GQ0/) wired to it. The DHT11 wiring is very simple:
- DHT11 `+` pin to the ESP8266 `VIN/5V` pin
- DHT11 `-` pin to an ESP8266 `GND` pin
- DHT11 `out` pin to ESP8266 `GPIO14` (marked `D5`)

To load code onto the ESP8266, connect it to your computer, enter a command line in the `sensor-node` directory and run:
```
make config-tools
make program
```
Once programming is complete, open a web browser to [http://unnamed.local/](http://unnamed.local/). This will show you a simple status page verifying that the temperature and humidity readings are working. Additionally, it allows you to rebrand the sensor node with a more useful name (e.g. "bedroom", "upstairs", etc.). This name will be used for the URL and MQTT topic, so it can't have any spaces or other weird characters in it.

Repeat these steps with as many sensors as you like, and they'll start publishing temperature and humidity data on your network.

## Server
To set up the server, cd into the `status-page` directory and run `python3 setup.py install`. This will install the python dependencies for you (assuming you already have python3 installed).

To manually launch the MQTT and HTTP servers, run:
```
python3 mqtt-client.py&
python3 status-page.py&
```
Once these are running, you should be able to view the status webpage at [http://localhost/](http://localhost/) or any other address that resolves to the server. All of the sensors on your network should automagically come up and show both their current readings and a time history.
