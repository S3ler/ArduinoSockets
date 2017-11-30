# arduino-linux-abstraction
This project provides fake implementation for Arduino frameworks to run on linux.

We only implement functionality needed for the mqtt-sn-gateway.

One interface is the SerialLinux class which fakes Arduino Serial print[https://www.arduino.cc/en/Serial/Print] commands to the standard unix tty.
We use it for logging to the console for the mqtt-sn gateway.
Additionally we cannot assume to use data and timestamp based logging on Arduinos. The Arduino Environment offers a timestamp via the millis() function[https://www.arduino.cc/en/Reference/Millis]. The millis() function returns the milliseconds since the Arduino or in our case the mqtt-sn gateway began running. The Arduino Environment returns a unsigned long which has 32 bits in the Arduino Environment [https://www.arduino.cc/en/Reference/UnsignedLong] our function on the other hand returns an uint64_t which uses 64 bits. This extends the maximum time until millis() returns an overflow but does not affect any behaviour within the implementations. The
The second interface is the SDLinux class which fakes Arduino's SD Card Library calls[https://www.arduino.cc/en/Reference/SD].
For this we used a ESP8266 and MH SD Card Module with the Arduino Environment for testing the exact behaviour of the SD Card Library by using the examples given by the Arduino Framework and then reimplement the behaviour into the SDLinux class.
The SD Card Library works with FAT32 and FAT32 file system. It must be considered that it only uses short 8.3 names for files, meaning the longest names for files is eight literals or numbers then a point and then additional three literals or numbers[https://support.microsoft.com/de-de/help/121007/how-to-disable-8-3-file-name-creation-on-ntfs-partitions]. This is of course a restriction we need to consider in the implementation of the Persistent interface.
We formatted the SD Card to FAT32 and build hardware configuration on a breadboard (see img/SDCardTestBoard.jpg)


Used hardware and software:

ESP8266 version
NodeMCU v2
https://nodemcu.readthedocs.io/en/master/en/
Datasheet
http://espressif.com/sites/default/files/documentation/0a-esp8266ex_datasheet_en.pdf

ESP8266 core for Arduino
https://github.com/esp8266/Arduino commit 507a15910e6c77e8608c9045e4ee36c106299b9b

SD Card Slot 
Produino Ams1117-3.3v SD Card Slot Reader Module
http://www.dx.com/de/p/produino-ams1117-3-3v-sd-card-slot-reader-module-w-sd-spi-works-with-arduino-official-boards-297664

SD Card
Integral INSDH4G4 V2NDB SDHC Class 3 - 4 GB

Arduino IDE and Framework
version 1.8.3


