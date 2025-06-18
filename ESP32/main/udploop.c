#include "udploop.h"

static int makeServerSocket(int * dst, struct sockaddr_in * addr)
{
    *dst = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (*dst < 0)
    {
        printf("Net: cannot create a new socket.\n");

        return 0;
    }

    int err = bind(*dst, (struct sockaddr *) addr,
                   sizeof(struct sockaddr_in));

    if (err < 0)
    {
        printf("Net: cannot bind a new socket.\n");

        return 0;
    }

    return 1;
}


int udploop_init(struct udploop * udploop)
{
    udploop->addr.sin_addr.s_addr = htonl(INADDR_ANY);
    udploop->addr.sin_family = AF_INET;
    udploop->addr.sin_port = htons(12345);

    if (!makeServerSocket(&udploop->fd,
                          &udploop->addr))
        return 0;

    return 1;
}


int udploop_receive(struct udploop * udploop)
{
    struct sockaddr_storage src_addr;
    socklen_t src_addr_len = sizeof(src_addr);

    int len = recvfrom(udploop->fd,
                       udploop->buffer,
                       sizeof(udploop->buffer),
                       0,
                       (struct sockaddr *) &src_addr, &src_addr_len);

    return len;
}


int udploop_wait(struct udploop * udploop, int timeout_us)
{
    fd_set read_set;

    FD_ZERO(&read_set);
    FD_SET(udploop->fd, &read_set);

    struct timeval timeval =
    {
        .tv_sec = 0,
        .tv_usec = timeout_us
    };

    int result = select(udploop->fd + 1, &read_set, NULL, NULL, &timeval);

    if (result < 0)
    {
        printf("udploop_wait: select failed, %s\n", strerror(errno));

        return -1;
    }

    if (result == 0)
        return 0;

    if (FD_ISSET(udploop->fd, &read_set))
        return 1;

    printf("udploop_wait: unexpected state\n");

    return -1;
}
