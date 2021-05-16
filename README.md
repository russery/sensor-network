# temp-network
Network of sensors to monitor temperature throughout the home


Raspberry Pi setup:
- sudo apt install mosquitto
- sudo apt install mosquitto-clients (maybe not necessary except for testing)
- Verify mosquitto is running and starts automatically?
- pip3 install paho-mqtt



TODO:
- downsample and write data to csv from all sensors
- Plot time history (hour, day, week) with max and min
- Periodically flush time history to flash? Maybe downsampled (average temp per hour?)