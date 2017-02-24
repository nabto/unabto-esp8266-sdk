#include "Nabto.h"

void NabtoClass::begin(const char* device_id,
                       const char* pre_shared_key)
{
    nabto_main_setup* nms = unabto_init_context();
    nms->id = device_id;

    nms->secureAttach = true;
    nms->secureData = true;
    nms->cryptoSuite = CRYPT_W_AES_CBC_HMAC_SHA256;

    const char *p;
    unsigned char *up;
    for(p = pre_shared_key, up = nms->presharedKey; *p ;p += 2, ++up) {
       *up = hctoi(p[0])*16 + hctoi(p[1]); // convert hex string to byte array
    }
    unabto_init();
}

void NabtoClass::version(char* version)
{
    sprintf(version, "%u.%u", RELEASE_MAJOR, RELEASE_MINOR);
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
