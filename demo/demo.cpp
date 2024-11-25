#include <minet/minet.h>

using namespace minet;

Ref<Logger> logger;

static void ping(const TextRequest& request, TextResponse& response)
{
    logger->Info("Ping request received:\n-----\n{}\n-----", request.Request().ToString());
    response.Text().append("pong");
}

static void echo(const TextRequest& request, JsonResponse& response)
{
    logger->Info("Echo request received:\n-----\n{}\n-----", request.Request().ToString());
    response.Json()["status"] = "ok";
    response.Json()["message"] = request.Text();
}

int main(int argc, char* argv[])
{
    WebHostBuilder builder;

    // Use custom app settings if provided.
    if (argc == 2)
    {
        builder.UseAppSettings(argv[1]);
    }
    else
    {
        builder.UseAppSettings();
    }

    // Get app logger.
    logger = builder.GetLogger("Demo");

    // Register handlers and run the server.
    builder.Get("/ping", RequestHandler<>::Bind(ping))
        .Post("/echo", RequestHandler<TextRequest, JsonResponse>::Bind(echo))
        .Build()
        ->Run();

    return 0;
}
