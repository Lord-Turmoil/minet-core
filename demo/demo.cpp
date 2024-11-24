#include <minet/minet.h>

using namespace minet;

void ping(const TextRequest& request, TextResponse& response)
{
    response.Text().append("pong\n");
}

int main()
{
    WebHostBuilder().Register("/ping", RequestHandler<>::Bind(ping)).Build()->Run();

    return 0;
}