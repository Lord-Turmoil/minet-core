#include "minet/utils/Native.h"
#include <unordered_map>

MINET_BEGIN

namespace native
{

struct SignalHandlerRegistration
{
    int Signal;
    bool Once;
    std::function<void()> Handler;
};

static std::unordered_map<int, SignalHandlerRegistration> sSignalHandlers;

static void _SignalHandler(int sig);

int SetSignalHandler(int sig, const std::function<void()>& handler, bool once)
{
    sSignalHandlers[sig] = { sig, once, handler };
    if (auto r = signal(sig, _SignalHandler); r == SIG_ERR)
    {
        return -1;
    }
    return 0;
}

int RemoveSignalHandler(int sig)
{
    sSignalHandlers.erase(sig);
    if (auto r = signal(sig, SIG_DFL); r == SIG_ERR)
    {
        return -1;
    }
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
        // FIXME: Error omitted.
        signal(sig, _SignalHandler);
    }
}

} // namespace native

MINET_END
