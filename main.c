#include <sys/socket.h>
#include <sys/epoll.h>
#include <linux/version.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 8)
#error "linux kernel before 2.6.8 is not supported"
#endif

#define ifailed(v, msg)       \
    if (v)                    \
    {                         \
        fprintf(stderr, msg); \
        exit(-1);             \
    }

void set_nonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    ifailed(flags < 0, "fcntl failed\n");
    ifailed(fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0, "fnctl set failed");
}

void epoll_ctl_helper(int epoll, int fd, int events, int opt)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(struct epoll_event));
    ev.events = events;
    ev.data.fd = fd;
    ifailed(epoll_ctl(epoll, opt, fd, &ev), "epoll_ctl failed");
}

int main(int argc, char **args)
{
    int epoll = epoll_create(1);
    ifailed(epoll < 0, "epoll_create failed\n");

    int listener = socket(AF_INET, SOCK_STREAM, 0);
    ifailed(listener < 0, "socket failed\n");
    set_nonblock(listener);
    struct sockaddr_in listen_addr;
    memset(&listen_addr, 0, sizeof(struct sockaddr_in));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = INADDR_ANY;
    listen_addr.sin_port = htons(8080);

    ifailed(
        bind(listener, (struct sockaddr *)&listen_addr, sizeof(listen_addr)),
        "bind failed\n");

    ifailed(listen(listener, 5), "listen failed\n");

    epoll_ctl_helper(epoll, listener, EPOLLIN, EPOLL_CTL_ADD);

    epoll_wait()

    close(listener);
    close(epoll);
}
