first_repos
===========

an attempt on creating a first repository

The project is not divided into a lot of functions at this point
but mainly consists of a 3-(4) case main program, which calls a few functions when reading on the One-Wire
which at this point is programmed for reading of temparature from a DS18B2C.

The project is set to run on the external makefile, why, this should be created by the user before running AVR.
There is a readme file in the write_Makefile folder that will instruct how to create the makefile

Afterwards, the object files can either be build in AVR or by the MAKE_ALL in the gnu make folder.

The program at this point uses the uart0 to show a Menu for the different functions implemented.
Theres an init/status mode that initialises GSM and GPS and sets the necessary flags and counters for the different cases. This case as well handles the reading of the last incoming GPS NMEA string and puts the latitude, longitude, utc time, date, satellite number into their right place in the SMS structure. When a message is requested to be sent, this case is briefly entered again (NOT RESET) to read the last GPS string, reading the One-Wire, packing the data, and sending the SMS.
When leaving this case one can choose between the 3 cases, GPS, GSM an I/O, from which, one at any time can reset to the init case (This disables GSM and GPS and re-initialises them).

The GPS case offers a read out of the entire NMEA string.

GSM case offers a range of options: Setting receiver number, creating custom sms, sequential sms sending to gateway and sending custom sms to set reciever.

The IO mode offers HIGH/LOW reading of the 4 inputs on the adc and setting ON/OFF output1
