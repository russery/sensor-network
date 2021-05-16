# temp-network
Network of sensors to monitor temperature throughout the home


Raspberry Pi setup:
- sudo apt install mosquitto
- sudo apt install mosquitto-clients (maybe not necessary except for testing)
- Verify mosquitto is running and starts automatically?
- pip3 install paho-mqtt



TODO:
- Branding process for sensors
	- Long press button to re-init
		- GPIO0 connected to Flash button
- Plot time history (hour, day, week) with max and min
- Periodically flush time history to flash? Maybe downsampled (average temp per hour?)