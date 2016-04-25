#include "Arduino.h"
#include "unabto_platform.h"

void p(const char *fmt, ... )
{
    static char tmp[128]; // resulting string limited to 128 chars.
    va_list args;
    va_start (args, fmt );
    vsnprintf(tmp, 128, fmt, args);
    va_end (args);
    Serial.println(tmp);
    delay(1);
}
