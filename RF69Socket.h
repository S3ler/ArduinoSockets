#ifndef RADIOHEADSOCKET_RF69SOCKET_H
#define RADIOHEADSOCKET_RF69SOCKET_H


#include <SocketInterface.h>
#include <RHReliableDatagram.h>
#include <RH_RF69.h>

#define OWN_ADDRESS 0x01
#define WAIT_PACKET_SEND_TIMEOUT 2000
// If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
// ishighpowermodule flag - then uncomment next line
// #define RFM69HW

class RF69Socket : public SocketInterface{
public:
    bool begin() override;

    void setRf69(RH_RF69 *rf69);

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
    RH_RF69* rf69;
    RHReliableDatagram* manager;

    device_address broadcastAddress;
    device_address ownAddress;

    LoggerInterface *logger;
    MqttSnMessageHandler *mqttSnMessageHandler;
};


#endif //RADIOHEADSOCKET_RF69SOCKET_H
