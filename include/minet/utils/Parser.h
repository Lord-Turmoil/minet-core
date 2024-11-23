#pragma once

#include <string>
#include "minet/common/Base.h"

MINET_BEGIN

struct HttpRequest;

namespace utils::http
{

/**
 * @brief Clean up the URL path.
 * If the URL path does not starts with '/', add one, and remove trailing '/'.
 * e.g. "abc" -> "/abc", "abc/" -> "/abc", "/abc" -> "/abc", "/abc/" -> "/abc"
 */
std::string CleanPath(const std::string& path);

/**
 * @brief Convert integer address to IP string.
 * @param address The integer address.
 * @param port The port.
 * @note If port is 0 or 80, will be ignored.
 */
std::string AddressToHost(uint32_t address, uint16_t port);

/**
 * @brief Parse HTTP request.
 * @param request The output request.
 * @return 0 on success, error codes are as follows:
 *   1: Failed to read from the stream.
 *   2: Failed to parse the prologue.
 *   3: Failed to parse one of the headers.
 */
int ParseHttpRequest(HttpRequest* request);

} // namespace utils::http

MINET_END
