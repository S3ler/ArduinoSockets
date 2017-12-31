// nrf95_ping_pong.ino
// -*- mode: C++ -*-

#ifdef ESP8266
#include <ESP8266WiFi.h>
// FROM: https://android.googlesource.com/platform/bionic/+/6861c6f/libc/string/strsep.c Date: 30.12.1017
/*
 * Get next token from string *stringp, where tokens are possibly-empty
 * strings separated by characters from delim.
 *
 * Writes NULs into the string at *stringp to end tokens.
 * delim need not remain constant from call to call.
 * On return, *stringp points past the last NUL written (if there might
 * be further tokens), or is NULL (if there are definitely no more tokens).
 *
 * If *stringp is NULL, strsep returns NULL.
 */
char *
strsep(char **stringp, const char *delim)
{
    char *s;
    const char *spanp;
    int c, sc;
    char *tok;
    if ((s = *stringp) == NULL)
        return (NULL);
    for (tok = s;;) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
    /* NOTREACHED */
}
#endif

#include <Arduino.h>

#ifdef Arduino_h

#include <SPI.h>
#include <Ethernet.h>

#endif


#include <LoggerInterface.h>
#include "MqttSnMessageHandler.h"
#include <RF95Socket.h>
#include <RH_NRF24.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>

#include <string.h>
#include <errno.h>
#include <limits.h>

RF95Socket socket;
LoggerInterface logger;


MqttSnMessageHandler mqttSnMessageHandler;


#ifdef DRIVER_RH_RF95
#ifdef ESP8266
RH_RF95 rh_driver(2, 15);
#else
RH_RF95 rh_driver;
#endif
#endif

#ifdef DRIVER_RH_NRF24
#ifdef ESP8266
// TODO
RH_NRF24 rh_driver(2, 15);
#else
RH_NRF24 rh_driver;
#endif
#endif

RHReliableDatagram manager(rh_driver);

#if defined(PING)
#define OWN_ADDRESS 0x05
#define PONG_ADDRESS 0x03
device_address target_address(PONG_ADDRESS, 0, 0, 0, 0, 0);
uint8_t msg[] = {5, 'P', 'i', 'n', 'g'};
#elif defined(PONG)
#define OWN_ADDRESS 0x03
#endif

enum ArduinoSocketTesterStatus {
    STARTING,
    IDLE,
    SEND,
    RECEIVE,
    PARSE_FAILURE,
    FAILURE,
    ERROR
};

enum SEND_STATUS {
    SEND_NONE,
    AWAIT_ADDRESS,
    AWAIT_DATA,
    SENDING
};

enum RECEIVE_STATUS {
    RECEIVE_NONE,
    SEND_ADDRESS,
    SEND_DATA,
};

ArduinoSocketTesterStatus status = STARTING;
RECEIVE_STATUS receive_status = RECEIVE_NONE;
SEND_STATUS send_status = SEND_NONE;
#define SerialBufferSize 1300
char serialBuffer[SerialBufferSize];
uint16_t serialBufferCounter = 0;
bool lineReady = false;

device_address destination_address;
uint8_t data[64];
uint16_t data_length = 0;


bool isSend(char *buffer);

bool isReceived(char *buffer);

bool parseAddress(char *buffer);

bool parseData(char *buffer);

bool sendDataToAddress();

void resetSendBuffer();

void resetSerialBuffer();

bool parseLong(const char *str, long *val);

bool send_receive_address();

bool send_receive_data();

void resetReceiveBuffer();

bool send_error();

bool isReset(char *buffer);

void setup() {
    Serial.begin(9600);
    Serial.print("ArduinoSocketTester VERSION ALPHA 0.01\n");

#ifdef DRIVER_RH_RF95
    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    if (!rh_driver.init()) {
        Serial.println("Failure init DRIVER_RH_RF95");
    }

#ifdef FREQUENCY
    if (!rh_driver.setFrequency(FREQUENCY)) {
        Serial.println("Failure set FREQUENCY");
    }
    Serial.println("FREQUENCY");
#endif

#ifdef TX_POWER_PIN
    if(!rh_driver.setTxPower(18, false)){
        Serial.println("Failure set TX_POWER_PIN");
    }
#endif

#ifdef MODEM_CONFIG_CHOICE
    if(!rh_driver.setModemConfig(RH_RF95::MODEM_CONFIG_CHOICE)){
        Serial.println("Failure set MODEM_CONFIG_CHOICE");
    }
    Serial.println("MODEM_CONFIG_CHOICE");
#endif
#endif
#ifdef DRIVER_RH_NRF24
    if (!rh_driver.init()) {
        Serial.println("Failure init DRIVER_RH_NRF24");
    }
    if (!rh_driver.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPowerm18dBm)) {
        Serial.println("Failure set DataRate250kbps, TransmitPowerm18dBm");
    }
#endif

    resetSendBuffer();
    resetSerialBuffer();

    status = STARTING;
    manager.setThisAddress(OWN_ADDRESS);
    socket.setManager(&manager);

    mqttSnMessageHandler.setLogger(&logger);
    mqttSnMessageHandler.setSocket(&socket);

    if (!mqttSnMessageHandler.begin()) {
        send_error();
        status = ERROR;
    } else {
        Serial.println("OK IDLE");
        status = IDLE;
    }


}


void loop() {
    if (Serial.available() > 0) {
        char c = Serial.read();
        serialBuffer[serialBufferCounter++] = c;
        if (c == '\n') {
            lineReady = true;
        } else if (serialBufferCounter == SerialBufferSize) {
            status = PARSE_FAILURE;
        }
    }
    mqttSnMessageHandler.loop();

    if (status == STARTING) {
        // init hardware components etc.
        setup();
    }
    if (status == IDLE && lineReady) {
        // parse
        if (isSend(serialBuffer)) {
            status = SEND;
            send_status = SEND_NONE;
            resetSerialBuffer();
        } else if (isReceived(serialBuffer)) {
            status = RECEIVE;
            receive_status = RECEIVE_NONE;
            resetSerialBuffer();
        } else if (isReset(serialBuffer)) {
            Serial.println("OK RESET");
#if defined(ESP8266)
            ESP.restart();
#else
            Serial.println("ERROR RESET_NOT_SUPPORTED");
#endif
        } else {
            status = PARSE_FAILURE;
        }

    }
    if (status == SEND) {
        if (send_status == SEND_NONE) {
            Serial.print("OK AWAIT_ADDRESS\n");
            resetSendBuffer();
            send_status = AWAIT_ADDRESS;
        } else if (send_status == AWAIT_ADDRESS && lineReady) {
            if (parseAddress(serialBuffer)) {
                Serial.print("OK AWAIT_DATA\n");
                resetSerialBuffer();
                send_status = AWAIT_DATA;
            } else {
                status = PARSE_FAILURE;
            }
        } else if (send_status == AWAIT_DATA && lineReady) {
            if (parseData(serialBuffer)) {
                Serial.print("OK SENDING\n");
                resetSerialBuffer();
                send_status = SENDING;
            } else {
                status = PARSE_FAILURE;
            }
        } else if (send_status == SENDING) {
            if (sendDataToAddress()) {
                Serial.print("OK IDLE\n");
                resetSendBuffer();
                send_status = SEND_NONE;
                status = IDLE;
            } else {
                Serial.print("FAILURE IDLE\n");
                send_status = SEND_NONE;
                status = FAILURE;
            }
        }
    }
    if (status == RECEIVE) {
        if (receive_status == RECEIVE_NONE) {
            Serial.print("OK SEND_ADDRESS\n");
            receive_status = SEND_ADDRESS;
        } else if (receive_status == SEND_ADDRESS) {
            if (send_receive_address()) {
                Serial.print("OK SEND_DATA\n");
                receive_status = SEND_DATA;
            } else {
                status = ERROR;
            }
        } else if (receive_status == SEND_DATA) {
            if (send_receive_data()) {
                Serial.print("OK IDLE\n");
                resetReceiveBuffer();
                receive_status = RECEIVE_NONE;
                status = IDLE;
            } else {
                status = ERROR;
            }
        }
    }
    if (status == PARSE_FAILURE) {
        Serial.print("FAILURE PARSE_FAILURE\n");
        resetSerialBuffer();
        receive_status = RECEIVE_NONE;
        send_status = SEND_NONE;
        status = IDLE;
    }
    if (status == FAILURE) {
        Serial.print("FAILURE\n");
        resetSerialBuffer();
        receive_status = RECEIVE_NONE;
        send_status = SEND_NONE;
        status = STARTING;
    }
    if (status == ERROR) {
        Serial.print("ERROR\n");
        // TODO reset chip completely (including peripheral)
    }

}

bool isReset(char *buffer) {
    char *token = strsep(&buffer, " ");
    if (token == NULL) {
        return false;
    }
    return memcmp(token, "RESET", strlen("RESET")) == 0;
}

void resetReceiveBuffer() {
    mqttSnMessageHandler.reset_received_buffer();
}

bool send_error() {
    Serial.print("ERROR\n");
    return true;
}

bool send_receive_data() {
    return mqttSnMessageHandler.print_received_data();
}

bool send_receive_address() {
    return mqttSnMessageHandler.print_received_address();
}

void resetSerialBuffer() {
    memset(serialBuffer, 0x0, sizeof(serialBuffer));
    serialBufferCounter = 0;
    lineReady = false;
}

void resetSendBuffer() {
    memset(&destination_address, 0x0, sizeof(device_address));
    memset(data, 0x0, sizeof(data));
    data_length = 0;
}

bool sendDataToAddress() {
    return mqttSnMessageHandler.send(&destination_address, data, data_length);
}

bool parseData(char *buffer) {
    char *token = strsep(&buffer, " ");
    if (token == NULL) {
        return false;
    }
    if (memcmp(token, "DATA", strlen("DATA")) != 0) {
        return false;
    }

    memset(data, 0x0, sizeof(data));
    data_length = 0;

    while ((token = strsep(&buffer, " ")) != NULL) {
        long int number = 0;
        if (!parseLong(token, &number)) {
            return false;
        }
        if (number > UINT8_MAX || number < 0) {
            return false;
        }
        data[data_length++] = (uint8_t) number;
    }
    return true;
}

bool parseAddress(char *buffer) {
    //Serial.println("parseAddress");
    char *token = strsep(&buffer, " ");
    if (token == NULL) {
        //Serial.println("token NULL");
        return false;
    }
    if (memcmp(token, "ADDRESS", strlen("ADDRESS")) != 0) {
        //Serial.println("does not start with ADDRESS");
        return false;
    }

    memset(&destination_address, 0x0, sizeof(device_address));
    uint16_t destination_address_length = 0;

    while ((token = strsep(&buffer, " ")) != NULL) {
        long int number = 0;
        if (!parseLong(token, &number)) {
            //Serial.println("failure parsing parseLong");
            return false;
        }
        //Serial.println(number, DEC);

        if (number > UINT8_MAX || number < 0) {
            //Serial.print("number out of bounds: ");
            //Serial.println(number, DEC);
            return false;
        }
        if (destination_address_length + 1 > sizeof(device_address)) {
            //Serial.println("address size too long");
            return false;
        }
        destination_address.bytes[destination_address_length++] = (uint8_t) number;
    }
    return destination_address_length == sizeof(device_address);
}

bool isReceived(char *buffer) {
    char *token = strsep(&buffer, " ");
    if (token == NULL) {
        return false;
    }
    return memcmp(token, "RECEIVE", strlen("RECEIVE")) == 0;
}

bool isSend(char *buffer) {
    char *token = strsep(&buffer, " ");
    if (token == NULL) {
        return false;
    }
    return memcmp(token, "SEND", strlen("SEND")) == 0;
}

bool parseLong(const char *str, long *val) {
    char *temp;
    bool rc = true;
    errno = 0;
    *val = strtol(str, &temp, 0);

    if (temp == str || (*temp != '\0' && *temp != '\n') ||
        ((*val == LONG_MIN || *val == LONG_MAX) && errno == ERANGE))
        rc = false;

    return rc;
}