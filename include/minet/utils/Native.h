#include <csignal>
#include <functional>
#include "minet/common/Base.h"

MINET_BEGIN

namespace native
{

/**
 * @brief Set the signal handler for the given signal.
 * @param sig The signal to set the handler.
 * @param handler The handler to set.
 * @param once Whether to remove the handler after the first call.
 * @return 0 on success, others on failure.
 * @note If the handler is already set, it will be replaced.
 */
int SetSignalHandler(int sig, const std::function<void(void)>& handler, bool once = true);

/**
 * @brief Remove the signal handler for the given signal.
 * @param sig The signal to remove the handler.
 * @return 0 on success, others on failure.
 */
int RemoveSignalHandler(int sig);

} // namespace native

MINET_END
