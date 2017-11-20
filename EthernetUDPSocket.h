//
// Created by bele on 18.11.17.
//

#ifndef RADIOHEADSOCKET_ethernetUDPSOCKET_H
#define RADIOHEADSOCKET_ethernetUDPSOCKET_H

#include <Ethernet.h>
#include <SocketInterface.h>
#include <IPAddress.h>
#include <EthernetUdp.h>

#define PORT 8888
#define ETHERNET_UDP_MAX_MESSAGE_LENGTH UINT8_MAX

class EthernetUDPSocket : public SocketInterface {
public:
    bool begin() override;

    void setEthernetUDP(EthernetUDP * ethernetUDP);

    void setLogger(LoggerInterface *logger) override;

    void setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) override;

    device_address *getBroadcastAddress() override;

    device_address *getAddress() override;

    uint8_t getMaximumMessageLength() override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) override;

    bool loop() override;

private:
    void convertToDeviceAddress(device_address *from, IPAddress fromIPAddress, uint16_t fromPort);
    bool handleEthernetUDPSocket();

private:
    EthernetUDP *ethernetUDP;
    IPAddress broadcastIPAddress;
    uint16_t broadcastPort;

    device_address broadcastAddress;
    device_address ownAddress;

    LoggerInterface *logger;
    MqttSnMessageHandler *mqttSnMessageHandler;
};


#endif //RADIOHEADSOCKET_ethernetUDPSOCKET_H
