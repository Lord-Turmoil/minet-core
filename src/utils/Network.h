/**
 * @author Tony S.
 * @details Network utility functions.
 */

#pragma once

#include "minet/common/Base.h"

#include <netinet/in.h>
#include <sys/types.h> // ssize_t

MINET_BEGIN

namespace network
{

struct AcceptData
{
    sockaddr_in Address;
    int SocketFd;
};

/**
 * @brief Open a socket to the given host and port.
 * @param port The port to connect to.
 * @param block Whether to set the socket to blocking mode.
 * @ref https://www.geeksforgeeks.org/socket-programming-cc/
 * @note
 * If block set to true, then @link accept @endlink will block
 * until a new connection arrives. This may hang the server
 * when interrupt signal received.
 */
int OpenSocket(uint16_t port, bool block = false);

/**
 * @brief Close the socket.
 * @param fd The file descriptor of the opened socket.
 * @return 0 on successful, others on failure.
 */
int CloseSocket(int fd);

/**
 * @brief Accept a new socket, i.e. request.
 * @param fd The listening socket file descriptor.
 * @param data Output the accepted data.
 * @return Whether there is a new data or not.
 * @warning If no new socket, the data will still be modified, and the SocketFd
 * will be -1.
 */
bool AcceptSocket(int fd, AcceptData* data);

/**
 * @brief Make the given socket non-blocking.
 * @param fd Existing socket fd.
 * @return 0 on success, < 0 on failure.
 */
int MakeNonBlockingSocket(int fd);

/**
 * @brief Read from the given socket.
 * @param fd Socket fd.
 * @param buffer Read buffer.
 * @param length Maximum size to read.
 * @return Bytes read. -1 if error.
 */
ssize_t ReadSocket(int fd, char* buffer, size_t length);

/**
 * @brief Write to the given socket.
 * @param fd Socket fd.
 * @param buffer Content to write.
 * @param length Length to write.
 * @return Bytes wrote. -1 if error.
 */
ssize_t WriteSocket(int fd, const char* buffer, size_t length);

/**
 * @brief Convert integer address to IP string.
 * @param address The integer address.
 * @param port The port.
 * @note If port is 0 or 80, will be ignored.
 */
std::string AddressToHost(uint32_t address, uint16_t port);

} // namespace network

MINET_END
