#include "utils/Epoll.h"

#include <unistd.h>

MINET_BEGIN

namespace epoll
{

int Create()
{
    return epoll_create1(0);
}

int Close(int epfd)
{
    return close(epfd);
}

int Monitor(int epfd, int fd, uint32_t events)
{
    epoll_event event;

    event.events = events;
    event.data.fd = fd;

    return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
}

int Monitor(int epfd, int fd, uint32_t events, void* data)
{
    epoll_event event;
    event.events = events;
    event.data.ptr = data;
    event.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
}

int Modify(int epfd, int fd, uint32_t events, void* data)
{
    epoll_event event;
    event.events = events;
    event.data.ptr = data;
    event.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
}

int Wait(int epfd, epoll_event* events, int maxEvents, int timeout)
{
    return epoll_wait(epfd, events, maxEvents, timeout);
}

} // namespace epoll

MINET_END
