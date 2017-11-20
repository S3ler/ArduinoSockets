//
// Created by bele on 18.11.17.
//

#include "EthernetUDPSocket.h"

bool EthernetUDPSocket::begin() {
    if (logger == nullptr) {
        return false;
    }
    if (mqttSnMessageHandler == nullptr) {
        return false;
    }
    if (ethernetUDP == nullptr) {
        return false;
    }

    broadcastIPAddress[0] = 224;
    broadcastIPAddress[1] = 0;
    broadcastIPAddress[2] = 0;
    broadcastIPAddress[3] = 0;
    broadcastPort = 1234;
    convertToDeviceAddress(&broadcastAddress, broadcastIPAddress, broadcastPort);

    return ethernetUDP->begin(PORT);
}


void EthernetUDPSocket::setEthernetUDP(EthernetUDP *ethernetUDP) {
    EthernetUDPSocket::ethernetUDP = ethernetUDP;
}

void EthernetUDPSocket::setLogger(LoggerInterface *logger) {
    EthernetUDPSocket::logger = logger;
}

void EthernetUDPSocket::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {
    EthernetUDPSocket::mqttSnMessageHandler = mqttSnMessageHandler;
}

device_address *EthernetUDPSocket::getBroadcastAddress() {
    return &this->broadcastAddress;
}

device_address *EthernetUDPSocket::getAddress() {
    IPAddress ownIPAddress = Ethernet.localIP();
    uint16_t ownPort = PORT;
    convertToDeviceAddress(&this->ownAddress, ownIPAddress, ownPort);
    return &this->ownAddress;
}

uint8_t EthernetUDPSocket::getMaximumMessageLength() {
    return ETHERNET_UDP_MAX_MESSAGE_LENGTH;
}

bool EthernetUDPSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    return send(destination, bytes, bytes_len, UINT8_MAX);
}

bool EthernetUDPSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {

    // convert to IPAddress and Port
    IPAddress destinationIPAddress(destination->bytes[0],
                                   destination->bytes[1],
                                   destination->bytes[2],
                                   destination->bytes[3]);
    uint16_t destinationPort = 0;
    memcpy(&destinationPort, &destination->bytes[4], 2);

    // send Packet
    ethernetUDP->beginPacket(destinationIPAddress, destinationPort);
    ethernetUDP->write(bytes, bytes_len);
    ethernetUDP->endPacket();

    return true;
}

bool EthernetUDPSocket::loop() {
    if (!handleEthernetUDPSocket()) {
        return false;
    }
    return true;
}

bool EthernetUDPSocket::handleEthernetUDPSocket() {
    uint8_t buf_len = 0;
    uint8_t buf[ETHERNET_UDP_MAX_MESSAGE_LENGTH];
    memset(&buf, 0x0, ETHERNET_UDP_MAX_MESSAGE_LENGTH);
    device_address from;
    memset(&from.bytes[0], 0x0, sizeof(device_address));

    uint16_t packetSize = ethernetUDP->parsePacket();

    if (packetSize) {
        if (packetSize > ETHERNET_UDP_MAX_MESSAGE_LENGTH) {
            // someone try to mess up the udp packet stream
            // read out the bytes and so discard the message
            for (uint16_t i = 0; i < (packetSize / ETHERNET_UDP_MAX_MESSAGE_LENGTH); i++) {
                ethernetUDP->read(buf, ETHERNET_UDP_MAX_MESSAGE_LENGTH);
            }
            if (packetSize % ETHERNET_UDP_MAX_MESSAGE_LENGTH > 0) {
                ethernetUDP->read(buf, static_cast<size_t>(packetSize % ETHERNET_UDP_MAX_MESSAGE_LENGTH));
            }
            return true;
        }
        uint16_t readSize = ethernetUDP->read(buf, ETHERNET_UDP_MAX_MESSAGE_LENGTH);
        IPAddress fromIPAddress = ethernetUDP->remoteIP();
        uint16_t fromPort = ethernetUDP->remoteIP();
        convertToDeviceAddress(&from, fromIPAddress, fromPort);

        mqttSnMessageHandler->receiveData(&from, buf);
    }
    return true;
}


void EthernetUDPSocket::convertToDeviceAddress(device_address *from, IPAddress fromIPAddress, uint16_t fromPort) {
    memcpy(&from->bytes[0], &fromIPAddress[0], 4);
    memcpy(&from->bytes[4], &fromPort, 2);
}
