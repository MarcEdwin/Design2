* remote_monitoring.ino
	- All modifications have been performed
	- No modifcations required
* NTPClient.cpp
	- No modifcations required
* NTPClient.h
	- No modifcations required
* bme280.cpp
	- Needs to be deleted and replaced with accelerometer file (don't which one)
* bme280.h
	- Needs to be deleted and replaced with accelerometer file (don't which one)
* iot_configs.h
	- Lines 10, 11, and 18 were modified to include SSID, password, and connection
	- Network settings in lines 10 and 11 must be updated when network is
	changed
	string.
* remote_monitoring.c
	- Line 8 include statement needs to be changed to use accelerometer file
	- Line 30 thermostat class needs to be changed to accelerometer class
	- A lot more stuff needs to be changed
* remote_monitoring.h
	- Function at line 11may need a modification if required
