#include <ESP8266WiFi.h>
#include <unabto/unabto_external_environment.h>

uint32_t resolved_v4addr;
nabto_dns_status_t resolve_dns_status = NABTO_DNS_NOT_FINISHED;

void nabto_dns_resolve(const char* id)
{
    IPAddress ip;
    if(WiFi.hostByName(id, ip))
        resolve_dns_status = NABTO_DNS_OK;
    else
        resolve_dns_status = NABTO_DNS_ERROR;
    resolved_v4addr = ip;
    resolved_v4addr = ntohl(resolved_v4addr);
}

nabto_dns_status_t nabto_dns_is_resolved(const char* id, uint32_t* v4addr)
{
    *v4addr = resolved_v4addr;
    NABTO_LOG_TRACE(("dns resolved: %03d.%03d.%03d.%03d", MAKE_IP_PRINTABLE(*v4addr)));
    return resolve_dns_status;
}
