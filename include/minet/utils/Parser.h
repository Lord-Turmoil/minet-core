#pragma once

#include <string>
#include "minet/common/Base.h"

MINET_BEGIN

struct HttpRequest;

namespace http
{

/**
 * @brief Clean up the URL path.
 * If the URL path does not start with '/', add one, and remove trailing '/'.
 * e.g. "abc" -> "/abc", "abc/" -> "/abc", "/abc" -> "/abc", "/abc/" -> "/abc"
 */
std::string CleanPath(const std::string& path);

/**
 * @brief Parse HTTP request.
 * @param request The output request.
 * @return 0 on success, error codes are as follows:
 *   1: Failed to read from the stream.
 *   2: Failed to parse the prologue.
 *   3: Failed to parse one of the headers.
 */
int ParseHttpRequest(HttpRequest* request);

} // namespace http

MINET_END
