#ifndef PLATFORMIOESP8266ENVIRONMENT_LOGGERINTERFACE_H
#define PLATFORMIOESP8266ENVIRONMENT_LOGGERINTERFACE_H

#include <stdint.h>
#include <Arduino.h>

class LoggerInterface {
private:
    uint8_t current_log_lvl = 2;
    uint8_t last_started_log_lvl = UINT8_MAX;
#ifndef Arduino_h
    SerialLinux Serial;
#endif

public:

     bool begin();

     void set_log_lvl(uint8_t log_lvl);

     void log(char *msg, uint8_t log_lvl);

     void log(const char *msg, uint8_t log_lvl);

     void start_log(char *msg, uint8_t log_lvl);

     void start_log(const char *msg, uint8_t log_lvl);

     void set_current_log_lvl(uint8_t log_lvl);

     void append_log(char *msg);

     void append_log(const char *msg);

};


#endif //PLATFORMIOESP8266ENVIRONMENT_LOGGERINTERFACE_H
