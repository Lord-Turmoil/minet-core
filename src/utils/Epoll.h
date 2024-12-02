#pragma once

#include "minet/common/Base.h"

#include <sys/epoll.h>

MINET_BEGIN

namespace epoll
{

/**
 * @brief Create a new epoll.
 * @return fd of the created epoll, -1 on failure
 */
int Create();

/**
 * @brief Close the epoll.
 * @param epfd The epoll fd.
 * @return 0 on success, -1 on failure.
 */
int Close(int epfd);

/**
 * @brief Add a new fd to epoll.
 * @param epfd Epoll fd.
 * @param fd The fd to monitor.
 * @param events The events to monitor.
 * @return 0 on success, -1 on failure.
 */
int Monitor(int epfd, int fd, uint32_t events);

/**
 * @brief Add a new fd to epoll.
 * @param epfd Epoll fd.
 * @param fd The fd to monitor.
 * @param events The events to monitor.
 * @param data The extra data for this event.
 * @return 0 on success, -1 on failure.
 */
int Monitor(int epfd, int fd, uint32_t events, void* data);

/**
 * @brief Remove a monitored fd from epoll.
 * @param epfd Epoll fd.
 * @param fd The fd to unmonitor.
 * @return 0 on success, -1 on failure.
 */
int Unmonitor(int epfd, int fd);

/**
 * @brief Modify a monitored epoll event.
 * @param epfd Epoll fd.
 * @param fd The fd.
 * @param events The events to monitor.
 * @param data The extra data for this event.
 * @return 0 on success, -1 on failure.
 */
int Modify(int epfd, int fd, uint32_t events, void* data);

/**
 * @brief Wait for epoll events.
 * @param epfd Wait for epoll events.
 * @param events The events buffer.
 * @param maxEvents Maximum events returned this time.
 * @param timeout The maximum wait time before return, by default 5 second.
 * @return The number of events returned. -1 on failure.
 * @warning Be careful when setting timeout to -1, it will block forever.
 */
int Wait(int epfd, epoll_event* events, int maxEvents, int timeout = 5000);

} // namespace epoll

MINET_END
