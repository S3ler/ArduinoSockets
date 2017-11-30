#ifndef RADIOHEADSOCKET_RF69SOCKET_H
#define RADIOHEADSOCKET_RF69SOCKET_H


#include <SocketInterface.h>
#include <RHReliableDatagram.h>
#include <RH_RF95.h>

#define OWN_ADDRESS 0x02

#define FREQUENCY 868
//#define TX_POWER_PIN 18
#define MODEM_CONFIG_CHOICE Bw31_25Cr48Sf512

class RF95Socket : public SocketInterface{
public:
    bool begin() override;

    void setRf95(RH_RF95 *rf95);

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
    RH_RF95* rf95;
    RHReliableDatagram* manager;

    device_address broadcastAddress;
    device_address ownAddress;

    LoggerInterface *logger;
    MqttSnMessageHandler *mqttSnMessageHandler;
};


#endif //RADIOHEADSOCKET_RF69SOCKET_H
