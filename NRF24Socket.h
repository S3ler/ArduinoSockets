//
// Created by bele on 17.11.17.
//

#ifndef RADIOHEADSOCKET_NRF24SOCKET_H
#define RADIOHEADSOCKET_NRF24SOCKET_H


#include <SocketInterface.h>

class NRF24Socket : public SocketInterface{
public:
    bool begin() override;

    void setLogger(LoggerInterface *logger) override;

    void setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) override;

    device_address *getBroadcastAddress() override;

    device_address *getAddress() override;

    uint8_t getMaximumMessageLength() override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) override;

    bool loop() override;
};


#endif //RADIOHEADSOCKET_NRF24SOCKET_H
