#ifndef RADIOHEADSOCKET_NRF24SOCKET_H
#define RADIOHEADSOCKET_NRF24SOCKET_H


#include <SocketInterface.h>
#include <RHReliableDatagram.h>
#include <RH_NRF24.h>

#define WAIT_PACKET_SEND_TIMEOUT 200

class NRF24Socket : public SocketInterface{

public:
    bool begin() override;

    void setNrf24(RH_NRF24 *nrf24);

    void setManager(RHReliableDatagram *manager);

    void setLogger(LoggerInterface *logger) override;

    void setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) override;

    device_address *getBroadcastAddress() override;

    device_address *getAddress() override;

    uint8_t getMaximumMessageLength() override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) override;

    bool loop() override;

private:
    RH_NRF24* nrf24;
    RHReliableDatagram* manager;

    device_address broadcastAddress;
    device_address ownAddress;

    LoggerInterface *logger;
    MqttSnMessageHandler *mqttSnMessageHandler;

};


#endif //RADIOHEADSOCKET_NRF24SOCKET_H
