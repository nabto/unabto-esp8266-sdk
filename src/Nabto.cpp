#include "Nabto.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <modules/util/read_hex.h>
#ifdef __cplusplus
} // extern "C"
#endif

void NabtoClass::begin(const char* device_id,
                       const char* pre_shared_key)
{
    nabto_main_setup* nms = unabto_init_context();
    nms->id = strdup(device_id);

    nms->secureAttach = true;
    nms->secureData = true;
    nms->cryptoSuite = CRYPT_W_AES_CBC_HMAC_SHA256;

    if (!unabto_read_psk_from_hex(pre_shared_key, nms->presharedKey, 16)) {
        NABTO_LOG_ERROR(("Invalid cryptographic key specified", pre_shared_key));
        return;
    }

    if (!unabto_init()) {
        NABTO_LOG_FATAL(("Failed at nabto_main_init"));
    }
}

void NabtoClass::version(char* version)
{
    sprintf(version, "%u.%u", RELEASE_MAJOR, RELEASE_MINOR);
}

void NabtoClass::tick()
{
    unabto_tick();
}

NabtoClass Nabto;