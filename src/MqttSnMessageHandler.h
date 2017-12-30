//
// Created by bele on 30.12.17.
//

#ifndef ARDUINOSOCKETTESTER_MQTTSNMESSAGEHANDLER_H
#define ARDUINOSOCKETTESTER_MQTTSNMESSAGEHANDLER_H


#include <SocketInterface.h>

class SocketInterface;

class MqttSnMessageHandler {
public:

    virtual bool begin();

    void setSocket(SocketInterface *socket);

    void setLogger(LoggerInterface *logger);

    virtual bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len);

    virtual void receiveData(device_address *address, uint8_t *bytes);

    virtual bool loop();

    bool print_received_address();

    bool print_received_data();

    void reset_received_buffer();


public:
    SocketInterface* socket = nullptr;
    LoggerInterface *logger = nullptr;

    device_address receive_address;
    uint8_t receive_buffer[64];
    uint16_t receive_buffer_length = 0;
};


#endif //ARDUINOSOCKETTESTER_MQTTSNMESSAGEHANDLER_H
