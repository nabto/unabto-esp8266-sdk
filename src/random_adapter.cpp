#include "Arduino.h"
#include <unabto/unabto_external_environment.h>

void nabto_random(uint8_t* buf, size_t len)
{
    size_t ix;
    for (ix = 0; ix < len; ++ix) {
        *buf++ = (uint8_t)random(0,255);
    }
}
