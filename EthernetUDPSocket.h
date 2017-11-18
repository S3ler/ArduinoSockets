//
// Created by bele on 18.11.17.
//

#ifndef RADIOHEADSOCKET_ETHERNETUDPSOCKET_H
#define RADIOHEADSOCKET_ETHERNETUDPSOCKET_H

#include <cstring>
#include <SocketInterface.h>

#ifdef ARDUINO
#include <Ethernet.h>
#include <EthernetUdp.h>
#endif

class EthernetUDP;

class Ethernet;

#define MAC 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
#define PORT 8888
#define ETHERNET_UDP_MAX_MESSAGE_LENGTH UINT8_MAX

class EthernetUDPSocket : public SocketInterface {

public:
    bool begin() override;

    void setEthernet(Ethernet *ethernet);

    void setEthernetUDP(EthernetUDP *ethernetUDP);

    void setLogger(LoggerInterface *logger) override;

    void setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) override;

    device_address *getBroadcastAddress() override;

    device_address *getAddress() override;

    uint8_t getMaximumMessageLength() override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) override;

    bool loop() override;

private:
    void convertToDeviceAddress(device_address *from, IPAddress *fromIPAddress, uint16_t fromPort);


private:
    uint8_t mac[] = {MAC};

    Ethernet *ethernet;
    EthernetUDP *ethernetUDP;

    device_address broadcastAddress;
    device_address ownAddress;

    LoggerInterface *logger;
    MqttSnMessageHandler *mqttSnMessageHandler;
};


#endif //RADIOHEADSOCKET_ETHERNETUDPSOCKET_H
