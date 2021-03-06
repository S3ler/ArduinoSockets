#ifndef RADIOHEADSOCKET_DUMMYSOCKET_H
#define RADIOHEADSOCKET_DUMMYSOCKET_H

#include <SocketInterface.h>
#include <iostream>

#warning "Compilation of the DummySocket - it has no functionality."
// TODO rewrite for Arduino not for C++ with STD
class DummySocket : public SocketInterface {
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

private:
    device_address broadcastAddress;
    device_address ownAddress;
};

#endif