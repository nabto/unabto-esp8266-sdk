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
    * @param ssid,         the SSID of the WiFi network
    * @param password,     the password for the WiFi network
    * @param nabtoId,      the Nabto ID of the device
    * @param presharedKey, the Nabto preshared key
    */
    void begin(const char* ssid, const char* password, const char* nabtoId, const char* presharedKey);

    /**
    * @param v, char array to get nabto version
    */
    void version(char* v);

    /**
    * Nabto main tick
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
