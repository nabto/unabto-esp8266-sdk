#include "Nabto.h"

void NabtoClass::begin(const char* ssid,
                       const char* password,
                       const char* nabtoId,
                       const char* presharedKey)
{
    // Initialize WiFi
    WiFi.begin(ssid, password);

    // Wait for connection
    NABTO_LOG_TRACE(("Waiting for connection..."));
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    NABTO_LOG_TRACE(("Connected to %s", ssid));

    // Initialize Nabto
    nabto_main_setup* nms = unabto_init_context();
    nms->ipAddress = ntohl(WiFi.localIP());
    nms->id = nabtoId;
    nms->secureAttach = true;
    nms->secureData = true;
#if NABTO_ENABLE_CONNECTIONS
    nms->cryptoSuite = CRYPT_W_AES_CBC_HMAC_SHA256;
#endif
    const char *p;
    unsigned char *up;
    for(p = presharedKey, up = nms->presharedKey; *p ;p += 2, ++up) {
       *up = hctoi(p[0])*16 + hctoi(p[1]); // convert hex string to byte array
    }
    unabto_init();
}

void NabtoClass::version(char* v)
{
    sprintf(v, "%u.%u", RELEASE_MAJOR, RELEASE_MINOR);
}

void NabtoClass::tick()
{
    unabto_tick();
}

int NabtoClass::hctoi(const char h)
{
    if(isdigit(h))
        return h - '0';
    else
        return toupper(h) - 'A' + 10;
}

NabtoClass Nabto;
