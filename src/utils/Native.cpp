#include "minet/utils/Native.h"
#include <unordered_map>

MINET_BEGIN

namespace native
{

struct SignalHandlerRegistration
{
    int Signal;
    std::function<void(void)> Handler;
    bool Once;
};

static std::unordered_map<int, SignalHandlerRegistration> sSignalHandlers;

static void _SignalHandler(int sig);

int SetSignalHandler(int sig, const std::function<void(void)>& handler, bool once)
{
    sSignalHandlers[sig] = { sig, handler, once };
    signal(sig, _SignalHandler);
    return 0;
}

int RemoveSignalHandler(int sig)
{
    sSignalHandlers.erase(sig);
    signal(sig, SIG_DFL);
    return 0;
}

void _SignalHandler(int sig)
{
    auto it = sSignalHandlers.find(sig);
    if (it == sSignalHandlers.end())
    {
        return;
    }

    it->second.Handler();
    if (it->second.Once)
    {
        sSignalHandlers.erase(it);
    }
    else
    {
        signal(sig, _SignalHandler);
    }
}

} // namespace native

MINET_END
