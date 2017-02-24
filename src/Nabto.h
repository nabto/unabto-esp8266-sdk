#ifndef _NABTO_H_
#define _NABTO_H_

#include "Arduino.h"

#include <ESP8266WiFi.h>

#include <unabto/unabto_common_main.h>
#include <unabto/unabto_app.h>
#include <unabto_version.h>

class NabtoClass
{
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

private:
    static int hctoi(const char h);
};

/**
* Default instance of Nabto.
*/
extern NabtoClass Nabto;

#endif
