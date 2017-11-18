# ArduinoSockets
ArduinoSockets implementations for MQTT-SN Gateway


### RadioHead
RadioHead Packet Radio library for embedded microprocessors.
We use Version 1.81 downloaded from the official [RadioHead side](http://www.airspayce.com/mikem/arduino/RadioHead/).
It is used for the NRF24Socket and the RF69Socket.

#### DummySocket
The DummySocket has no functionality it only prints out messages what he does so we you can see how the SocketInterface is to be implemented.

#### EthernetUDPSocket
The EthernetUDPSocket cannot receive Multicast UDP Packet due to the Arduino's Ethernet Library.

