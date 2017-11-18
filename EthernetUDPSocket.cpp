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
    if (ethernet == nullptr) {
        return false;
    }
    if (ethernetUDP == nullptr) {
        return false;
    }
    IPAddress broadcastAddress(224, 0, 0, 0);
    uint16_t broadcastPort = 1234;
    convertToDeviceAddress(&broadcastAddress, &broadcastAddress, broadcastPort);

    if (ethernet->begin(mac, ip)) {
        return false;
    }
    return ethernetUDP->begin(PORT);
}

void EthernetUDPSocket::setEthernet(Ethernet *ethernet) {
    EthernetUDPSocket::ethernet = ethernet;
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
    return &broadcastAddress;
}

device_address *EthernetUDPSocket::getAddress() {
    IPAddress ownIPAddress = ethernet->localIP();
    uint16_t ownPort = PORT;
    convertToDeviceAddress(&ownAddress, &ownIPAddress, ownPort);
    return &ownAddress;
}

uint8_t EthernetUDPSocket::getMaximumMessageLength() {
    return ETHERNET_UDP_MAX_MESSAGE_LENGTH;
}

bool EthernetUDPSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    return send(destination, bytes, bytes_len, UINT8_MAX);
}

bool EthernetUDPSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {
    if (destination == &broadcastAddress) {
        IPAddress destinationIPAddress(destination->bytes[0], destination->bytes[1], destination->bytes[2],
                                       destination->bytes[3]);
        uint16_t destinationPort = 0;
        memcpy(&destinationPort, &destination->bytes[4], 2);
        ethernetUDP->beginMulticast(destinationIPAddress, destinationPort);
        ethernetUDP->write(bytes, bytes_len);
        ethernetUDP->endPacket();
        return true;
    }

    // convert to IPAddress and Port
    IPAddress destinationIPAddress(destination->bytes[0], destination->bytes[1], destination->bytes[2],
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
                ethernetUDP->read(buf, packetSize % ETHERNET_UDP_MAX_MESSAGE_LENGTH);
            }
            return true;
        }

        IPAddress fromIPAddress = ethernetUDP->remoteIP();
        uint16_t fromPort = ethernetUDP->remotePort();
        buf_len = ethernetUDP->read(buf, ETHERNET_UDP_MAX_MESSAGE_LENGTH);
        convertToDeviceAddress(&from, &fromIPAddress, fromPort);
#ifdef GATEWAY_MQTTSNMESSAGEHANDLER_H
        mqttSnMessageHandler->receiveData(&from, &buf);
#elif CLIENT_MQTTSNMESSAGEHANDLER_H
        mqttSnMessageHandler->receiveData(&from, &buf);
#else
#warning "Compilation without Core MQTT-SN Gateway."
#endif

    }
    return true;
}

void EthernetUDPSocket::convertToDeviceAddress(device_address *from, IPAddress *fromIPAddress, uint16_t fromPort) {
    memcpy(&from->bytes[0], &fromIPAddress->uint32_t(), 4);
    memcpy(&from->bytes[4], &fromPort, 2);
}
