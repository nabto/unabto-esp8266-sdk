#include <WiFiUdp.h>
#include <unabto/unabto_external_environment.h>

#define MAXIMUM_NUMBER_OF_SOCKETS NABTO_CONNECTIONS_SIZE

enum internal_socket_state
{
    INTERNAL_SOCKET_STATE_FREE,
    INTERNAL_SOCKET_STATE_OPEN_UDP
};

struct internal_socket
{
    internal_socket_state state = INTERNAL_SOCKET_STATE_FREE;
    uint16_t localPort;
    WiFiUDP* udp = 0;
};

static internal_socket sockets[MAXIMUM_NUMBER_OF_SOCKETS];

#define LOWEST_UDP_PORT 51000

static uint16_t get_free_udp_port()
{
    static uint16_t lastPort = LOWEST_UDP_PORT - 1;
    uint8_t i = 0;

    lastPort++;

    while(i < MAXIMUM_NUMBER_OF_SOCKETS)
    {
        if(sockets[i].state == INTERNAL_SOCKET_STATE_OPEN_UDP && sockets[i].localPort == lastPort)
        {
            if(++lastPort == 0)
                lastPort = LOWEST_UDP_PORT;
            i = 0; // start over the is-port-in-use test
        }
        else
        {
            i++;
        }
    }
    return lastPort;
}

bool nabto_init_socket(uint32_t localAddr, uint16_t* localPort, nabto_socket_t* socket)
{
    bool result = false;
    NABTO_NOT_USED(localAddr);

    uint8_t i;
    for(i = 0; i < MAXIMUM_NUMBER_OF_SOCKETS; i++)
    {
        if(sockets[i].state == INTERNAL_SOCKET_STATE_FREE)
        {
            // if source port was set to 0 find and return an unused port
            if(*localPort == 0)
                *localPort = get_free_udp_port();

            if(sockets[i].udp == 0)
                sockets[i].udp = new WiFiUDP();

            if(sockets[i].udp->begin(*localPort))
            {
                sockets[i].localPort = *localPort;
                sockets[i].state = INTERNAL_SOCKET_STATE_OPEN_UDP;

                *socket = (nabto_socket_t) i;
                result = true;
            }

            break;
        }
    }

    NABTO_LOG_TRACE(("nabto_init_socket socket=%u localPort=%u", (uint8_t) *socket, *localPort));
    return result;
}

void nabto_close_socket(nabto_socket_t* socket)
{
    NABTO_LOG_TRACE(("nabto_close_socket socket=%u", (uint8_t) *socket));

    uint8_t i = (uint8_t) * socket;
    if(i < MAXIMUM_NUMBER_OF_SOCKETS && sockets[i].state == INTERNAL_SOCKET_STATE_OPEN_UDP)
    {
        if(sockets[i].udp != 0)
            sockets[i].udp->stop();
        sockets[i].state = INTERNAL_SOCKET_STATE_FREE;
    }
}

ssize_t nabto_read(nabto_socket_t socket,
                   uint8_t*       buf,
                   size_t         maxLength,
                   uint32_t*      addr,
                   uint16_t*      port)
{
    uint16_t len = 0;

    uint8_t i = (uint8_t) socket;
    if(i < MAXIMUM_NUMBER_OF_SOCKETS && sockets[i].state == INTERNAL_SOCKET_STATE_OPEN_UDP)
    {
        len = sockets[i].udp->parsePacket();
        if(len)
        {
            *addr = sockets[i].udp->remoteIP();
            *addr  = ntohl(*addr);
            *port = sockets[i].udp->remotePort();
            sockets[i].udp->read(buf, maxLength);

            NABTO_LOG_TRACE(("nabto_read socket=%u length=%u addr=%03d.%03d.%03d.%03d port=%u",
                (uint8_t) socket, len, MAKE_IP_PRINTABLE(*addr), port));
        }
    }

    return len;
}

ssize_t nabto_write(nabto_socket_t socket,
                    const uint8_t* buf,
                    size_t         len,
                    uint32_t       addr,
                    uint16_t       port)
{
    uint8_t i = (uint8_t) socket;
    if(i < MAXIMUM_NUMBER_OF_SOCKETS && sockets[i].state == INTERNAL_SOCKET_STATE_OPEN_UDP)
    {
        if(!sockets[i].udp->beginPacket(htonl(addr), port))
            return -1;

        sockets[i].udp->write(buf, len);

        if(!sockets[i].udp->endPacket())
            return -1;
    }

    NABTO_LOG_TRACE(("nabto_write socket=%u length=%u addr=%03d.%03d.%03d.%03d port=%u",
        (uint8_t) socket, len, MAKE_IP_PRINTABLE(addr), port));

    return len;
}
