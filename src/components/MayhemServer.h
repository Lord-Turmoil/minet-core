#pragma once

#include "components/BasicServer.h"

#include "threading/ThreadPool.h"

MINET_BEGIN

/**
 * @brief Mayhem server use thread pool for async processing.
 */
class MayhemServer final : public BasicServer
{
public:
    explicit MayhemServer(const Ref<ServerConfig>& config);
    ~MayhemServer() override = default;

    MayhemServer(const MayhemServer&) = delete;
    MayhemServer& operator=(const MayhemServer&) = delete;
    MayhemServer(MayhemServer&&) noexcept = default;
    MayhemServer& operator=(MayhemServer&&) noexcept = default;

    static const char* Identifier()
    {
        return "Mayhem";
    }

    const char* Name() const override
    {
        return Identifier();
    }

protected:
    void _OnNewConnection(const network::AcceptData& data) override;

private:
    void _HandleConnection(const network::AcceptData& data);

private:
    threading::ThreadPool _threadPool;
};

MINET_END
