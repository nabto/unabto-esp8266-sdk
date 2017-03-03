#ifndef _NABTO_H_
#define _NABTO_H_

#include "Arduino.h"

#include <ESP8266WiFi.h>

#include <unabto/unabto_common_main.h>
#include <unabto/unabto_app.h>
#include <unabto_version.h>

class NabtoClass {
 public:
    /**
    * @param device_id,       the unique device ID
    * @param pre_shared_key,  the pre-shared crypto key
    */
    void begin(const char* device_id, const char* pre_shared_key);

    /**
    * @param v,  string containing the uNabto version
    */
    void version(char* version);

    /**
    * uNabto tick
    * (Call approx. every 10ms.)
    */
    void tick();

    application_event_result copy_buffer(unabto_query_request* read_buffer,
                                         uint8_t* dest, uint16_t bufSize,
                                         uint16_t* len) const;

    application_event_result copy_string(unabto_query_request* read_buffer,
                                         char* dest, uint16_t destSize) const;

    bool write_string(unabto_query_response* write_buffer,
                      const char* string) const;
};

/**
* Default instance of Nabto.
*/
extern NabtoClass Nabto;

#endif
