#include "minet/common/Base.h"

MINET_BEGIN

namespace http
{

namespace status
{
// clang-format off
constexpr int OK                    = 200;
constexpr int NO_CONTENT            = 204;
constexpr int BAD_REQUEST           = 400;
constexpr int NOT_FOUND             = 404;
constexpr int INTERNAL_SERVER_ERROR = 500;
// clang-format on
} // namespace status

namespace entities
{
// clang-format off
constexpr char HTTP_VERSION[]       = "HTTP/1.1";
constexpr char SPACE[]              = " ";
constexpr char COLON[]              = ": ";
constexpr char NEW_LINE[]           = "\r\n";
constexpr char CONTENT_TYPE[]       = "Content-Type";
constexpr char CONTENT_LENGTH[]     = "Content-Length";
// clang-format on
} // namespace entities

/**
 * @brief Get corresponding description of HTTP status codes.
 * @param statusCode HTTP status code.
 * @return Description of the status code.
 */
const char* StatusCodeToDescription(int statusCode);

} // namespace http

MINET_END
