#include "minet/utils/UrlUtils.h"

MINET_BEGIN

namespace utils
{

std::string CleanUrl(const std::string& url)
{
    std::string cleanedUrl = url;
    if (cleanedUrl.front() != '/')
    {
        cleanedUrl = "/" + cleanedUrl;
    }
    while (cleanedUrl.back() == '/')
    {
        cleanedUrl.pop_back();
    }
    return cleanedUrl;
}

} // namespace utils

MINET_END
