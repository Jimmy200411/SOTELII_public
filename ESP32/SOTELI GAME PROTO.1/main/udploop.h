#ifndef udploop_h
#define udploop_h

#include "lwip/sockets.h"

struct udploop
{
    int fd;

    struct sockaddr_in addr;

    uint8_t buffer[64];
};

int udploop_init(struct udploop * udploop);

int udploop_receive(struct udploop * udploop);

int udploop_wait(struct udploop * udploop, int timeout_ms);

#endif
