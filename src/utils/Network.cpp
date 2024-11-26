#include "utils/Network.h"

#include "minet/common/Assert.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

MINET_BEGIN

namespace network
{

int OpenSocket(uint16_t port, bool block)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        return fd;
    }

    // Here, we do not reuse the address and port.
    int opt = 1;
    MINET_TRY_WITH_ACTION(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)), close(fd));

    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    MINET_TRY_WITH_ACTION(bind(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)), close(fd));

    MINET_TRY_WITH_ACTION(listen(fd, 2048), close(fd));

    if (!block)
    {
        MINET_TRY_WITH_ACTION(MakeNonBlockingSocket(fd), close(fd));
    }

    return fd;
}

int CloseSocket(int fd)
{
    return close(fd);
}

bool AcceptSocket(int fd, AcceptData* data)
{
    // This is annoying, but we have to do this.
    static socklen_t size = sizeof(sockaddr_in);

    MINET_ASSERT(data);

    data->SocketFd = accept(fd, reinterpret_cast<sockaddr*>(&data->Address), &size);

    return data->SocketFd > 0;
}

int MakeNonBlockingSocket(int fd)
{
    MINET_ASSERT(fd > 0);

    int flag = fcntl(fd, F_GETFL, 0);
    if (flag < 0)
    {
        return flag;
    }

    flag |= O_NONBLOCK;
    MINET_TRY(fcntl(fd, F_SETFL, flag));

    return 0;
}

ssize_t ReadSocket(int fd, char* buffer, size_t length)
{
    return recv(fd, buffer, length, 0);
}

ssize_t WriteSocket(int fd, const char* buffer, size_t length)
{
    return send(fd, buffer, length, 0);
}

std::string AddressToHost(uint32_t address, uint16_t port)
{
    address = htonl(address);
    std::string host = std::to_string((address >> 24) & 0xFF) + "." + std::to_string((address >> 16) & 0xFF) + "." +
                       std::to_string((address >> 8) & 0xFF) + "." + std::to_string(address & 0xFF);
    if ((port != 0) && (port != 80))
    {
        host += ":" + std::to_string(port);
    }
    return host;
}

} // namespace network

MINET_END
