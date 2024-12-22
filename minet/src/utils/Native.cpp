#include "utils/Native.h"

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
    struct sigaction act;
    act.sa_handler = _SignalHandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    if (once)
    {
        act.sa_flags |= SA_RESETHAND;
    }
    MINET_TRY(sigaction(sig, &act, nullptr));

    sSignalHandlers[sig] = { sig, once, handler };

    return 0;
}

int RemoveSignalHandler(int sig)
{
    sSignalHandlers.erase(sig);

    struct sigaction act;
    act.sa_handler = SIG_DFL;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    MINET_TRY(sigaction(sig, &act, nullptr));

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
}

} // namespace native

MINET_END
