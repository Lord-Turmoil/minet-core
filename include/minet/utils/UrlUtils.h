#include "minet/common/Base.h"

MINET_BEGIN

namespace utils
{

/**
 * @brief Clean up the URL path.
 * If the URL path does not starts with '/', add one, and remove trailing '/'.
 * e.g. "abc" -> "/abc", "abc/" -> "/abc", "/abc" -> "/abc", "/abc/" -> "/abc"
 */
std::string CleanUrl(const std::string& url);

} // namespace utils

MINET_END
