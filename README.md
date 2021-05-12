# temp-network
Network of sensors to monitor temperature throughout the home


TODO:
- MDNS lookup for other sensors on network
	- If none found, become primary and host webpage, while continuing to poll for other sensors
	- If found, become secondary and publish temperature (how? GET request to primary?)
- Branding process for sensors
	- Allow renaming sensors from master webpage
- Handle sensors that go on and offline cleanly
- Plot time history (hour, day, week) with max and min
- Periodically flush time history to flash? Maybe downsampled (average temp per hour?)