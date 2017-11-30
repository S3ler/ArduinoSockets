//
// Created by bele on 18.11.17.
//

#ifndef RADIOHEADSOCKET_WIFIUDPSOCKET_H
#define RADIOHEADSOCKET_WIFIUDPSOCKET_H
#ifdef ESP8266

#include <ESP8266WiFi.h>
#include <SocketInterface.h>
#include <WiFiUdp.h>
#include <IPAddress.h>
#define PORT 8888
#define ESP8266_UDP_MAX_MESSAGE_LENGTH UINT8_MAX

class ESP8266UDPSocket : public SocketInterface {
public:
    bool begin() override;

    void setWiFiUDP(WiFiUDP *wiFiUDP, WiFiUDP *multicastUDP);

    void setLogger(LoggerInterface *logger) override;

    void setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) override;

    device_address *getBroadcastAddress() override;

    device_address *getAddress() override;

    uint8_t getMaximumMessageLength() override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) override;

    bool loop() override;

    void convertToDeviceAddress(device_address *from, IPAddress fromIPAddress, uint16_t fromPort);

private:
    bool handleWifiUDPSocket();
    bool handleMulticastUDPSocket();

private:
    WiFiUDP *wiFiUDP;
    IPAddress broadcastIPAddress;
    uint16_t broadcastPort;

    device_address broadcastAddress;
    device_address ownAddress;

    LoggerInterface *logger;
    MqttSnMessageHandler *mqttSnMessageHandler;
    WiFiUDP *multicastUDP;
};

#endif

#endif //RADIOHEADSOCKET_WIFIUDPSOCKET_H
