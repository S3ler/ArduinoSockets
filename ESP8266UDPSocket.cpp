//
// Created by bele on 18.11.17.
//

#include "ESP8266UDPSocket.h"

bool ESP8266UDPSocket::begin() {
    if (logger == nullptr) {
        return false;
    }
    if (mqttSnMessageHandler == nullptr) {
        return false;
    }
    if (wiFiUDP == nullptr) {
        return false;
    }
    if (multicastUDP == nullptr) {
        return false;
    }

    broadcastIPAddress[0] = 224;
    broadcastIPAddress[1] = 0;
    broadcastIPAddress[2] = 0;
    broadcastIPAddress[3] = 0;
    broadcastPort = 1234;

    if (!wiFiUDP->begin(PORT)) {
        return false;
    }

    yield();
    delay(500);
    convertToDeviceAddress(&broadcastAddress, broadcastIPAddress, broadcastPort);
    if (!multicastUDP->beginMulticast(WiFi.localIP(), broadcastIPAddress, broadcastPort)) {
        return false;
    }
    return true;
}


void ESP8266UDPSocket::setWiFiUDP(WiFiUDP *wiFiUDP, WiFiUDP *multicastUDP) {
    ESP8266UDPSocket::wiFiUDP = wiFiUDP;
    ESP8266UDPSocket::multicastUDP = multicastUDP;
}

void ESP8266UDPSocket::setLogger(LoggerInterface *logger) {
    ESP8266UDPSocket::logger = logger;
}

void ESP8266UDPSocket::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {
    ESP8266UDPSocket::mqttSnMessageHandler = mqttSnMessageHandler;
}

device_address *ESP8266UDPSocket::getBroadcastAddress() {
    return &this->broadcastAddress;
}

device_address *ESP8266UDPSocket::getAddress() {
    IPAddress ownIPAddress = WiFi.localIP();
    uint16_t ownPort = PORT;
    convertToDeviceAddress(&this->ownAddress, ownIPAddress, ownPort);
    return &this->ownAddress;
}

uint8_t ESP8266UDPSocket::getMaximumMessageLength() {
    return ESP8266_UDP_MAX_MESSAGE_LENGTH;
}

bool ESP8266UDPSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    return send(destination, bytes, bytes_len, UINT8_MAX);
}

bool ESP8266UDPSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {
    if (destination == &broadcastAddress) {
        IPAddress destinationIPAddress(destination->bytes[0], destination->bytes[1], destination->bytes[2],
                                       destination->bytes[3]);
        uint16_t destinationPort = 0;
        memcpy(&destinationPort, &destination->bytes[4], 2);

        Serial.println(destinationIPAddress);
        Serial.println(destinationPort);

        wiFiUDP->beginPacketMulticast(broadcastIPAddress, broadcastPort, WiFi.localIP(), 1);
        wiFiUDP->write(bytes, bytes_len);
        wiFiUDP->endPacket();
        return true;
    }

    // convert to IPAddress and Port
    IPAddress destinationIPAddress(destination->bytes[0], destination->bytes[1], destination->bytes[2],
                                   destination->bytes[3]);
    uint16_t destinationPort = 0;
    memcpy(&destinationPort, &destination->bytes[4], 2);

    // send Packet
    wiFiUDP->beginPacket(destinationIPAddress, destinationPort);
    wiFiUDP->write(bytes, bytes_len);
    wiFiUDP->endPacket();

    return true;
}

bool ESP8266UDPSocket::loop() {
    if (!handleWifiUDPSocket()) {
        return false;
    }
    if (!handleMulticastUDPSocket()) {
        return false;
    }
    return true;
}

bool ESP8266UDPSocket::handleWifiUDPSocket() {
    uint8_t buf_len = 0;
    uint8_t buf[ESP8266_UDP_MAX_MESSAGE_LENGTH];
    memset(&buf, 0x0, ESP8266_UDP_MAX_MESSAGE_LENGTH);
    device_address from;
    memset(&from.bytes[0], 0x0, sizeof(device_address));

    uint16_t packetSize = wiFiUDP->parsePacket();

    if (packetSize) {
        if (packetSize > ESP8266_UDP_MAX_MESSAGE_LENGTH) {
            // someone try to mess up the udp packet stream
            // read out the bytes and so discard the message
            for (uint16_t i = 0; i < (packetSize / ESP8266_UDP_MAX_MESSAGE_LENGTH); i++) {
                wiFiUDP->read(buf, ESP8266_UDP_MAX_MESSAGE_LENGTH);
            }
            if (packetSize % ESP8266_UDP_MAX_MESSAGE_LENGTH > 0) {
                wiFiUDP->read(buf, static_cast<size_t>(packetSize % ESP8266_UDP_MAX_MESSAGE_LENGTH));
            }
            return true;
        }
        uint16_t readSize = wiFiUDP->read(buf, ESP8266_UDP_MAX_MESSAGE_LENGTH);
        IPAddress fromIPAddress = wiFiUDP->remoteIP();
        uint16_t fromPort = wiFiUDP->remoteIP();
        convertToDeviceAddress(&from, fromIPAddress, fromPort);

        mqttSnMessageHandler->receiveData(&from, buf);
    }
    return true;
}

bool ESP8266UDPSocket::handleMulticastUDPSocket() {
    uint8_t buf_len = 0;
    uint8_t buf[ESP8266_UDP_MAX_MESSAGE_LENGTH];
    memset(&buf, 0x0, ESP8266_UDP_MAX_MESSAGE_LENGTH);
    device_address from;
    memset(&from.bytes[0], 0x0, sizeof(device_address));

    uint16_t packetSize = multicastUDP->parsePacket();

    if (packetSize) {
        if (packetSize > ESP8266_UDP_MAX_MESSAGE_LENGTH) {
            // someone try to mess up the udp packet stream
            // read out the bytes and so discard the message
            for (uint16_t i = 0; i < (packetSize / ESP8266_UDP_MAX_MESSAGE_LENGTH); i++) {
                multicastUDP->read(buf, ESP8266_UDP_MAX_MESSAGE_LENGTH);
            }
            if (packetSize % ESP8266_UDP_MAX_MESSAGE_LENGTH > 0) {
                multicastUDP->read(buf, static_cast<size_t>(packetSize % ESP8266_UDP_MAX_MESSAGE_LENGTH));
            }
            return true;
        }
        uint16_t readSize = multicastUDP->read(buf, ESP8266_UDP_MAX_MESSAGE_LENGTH);
        IPAddress fromIPAddress = multicastUDP->remoteIP();
        uint16_t fromPort = multicastUDP->remoteIP();
        convertToDeviceAddress(&from, fromIPAddress, fromPort);

        mqttSnMessageHandler->receiveData(&from, buf);
    }
    return true;
}

void ESP8266UDPSocket::convertToDeviceAddress(device_address *from, IPAddress fromIPAddress, uint16_t fromPort) {
    memcpy(&from->bytes[0], &fromIPAddress[0], 4);
    memcpy(&from->bytes[4], &fromPort, 2);
}
