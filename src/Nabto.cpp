#include "Nabto.h"

extern "C" {
#include <modules/util/read_hex.h>
}

void NabtoClass::begin(const char* device_id, const char* pre_shared_key) {
    nabto_main_setup* nms = unabto_init_context();
    nms->id = strdup(device_id);

    nms->secureAttach = true;
    nms->secureData = true;
    nms->cryptoSuite = CRYPT_W_AES_CBC_HMAC_SHA256;

    if (!unabto_read_psk_from_hex(pre_shared_key, nms->presharedKey, 16)) {
        NABTO_LOG_ERROR(
            ("Invalid cryptographic key specified", pre_shared_key));
        return;
    }

    if (!unabto_init()) {
        NABTO_LOG_FATAL(("Failed at nabto_main_init"));
    }
}

void NabtoClass::version(char* version) {
    sprintf(version, "%u.%u", RELEASE_MAJOR, RELEASE_MINOR);
}

void NabtoClass::tick() {
    unabto_tick();
}

application_event_result NabtoClass::copy_buffer(
    unabto_query_request* read_buffer, uint8_t* dest, uint16_t bufSize,
    uint16_t* len) const {
    uint8_t* buffer;
    if (!(unabto_query_read_uint8_list(read_buffer, &buffer, len))) {
        return AER_REQ_TOO_SMALL;
    }
    if (*len > bufSize) {
        return AER_REQ_TOO_LARGE;
    }
    memcpy(dest, buffer, *len);
    return AER_REQ_RESPONSE_READY;
}

application_event_result NabtoClass::copy_string(
    unabto_query_request* read_buffer, char* dest, uint16_t destSize) const {
    uint16_t len;
    application_event_result res =
        copy_buffer(read_buffer, (uint8_t*)dest, destSize - 1, &len);
    if (res != AER_REQ_RESPONSE_READY) {
        return res;
    }
    dest[len] = 0;
    return AER_REQ_RESPONSE_READY;
}

bool NabtoClass::write_string(unabto_query_response* write_buffer,
                              const char* string) const {
    return unabto_query_write_uint8_list(write_buffer, (uint8_t*)string,
                                         strlen(string));
}

NabtoClass Nabto;
