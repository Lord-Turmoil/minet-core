# minet core

> A C++ HTTP server library mimicking ASP.NET Core
>
> Copyright &copy; 2024 Tony's Studio

---

[![Build-and-Test](https://github.com/Lord-Turmoil/minet-core/actions/workflows/build-and-test.yml/badge.svg?branch=main)](https://github.com/Lord-Turmoil/minet-core/actions/workflows/build-and-test.yml)

# Prologue

**minet core** is a C++ HTTP server library that mimics the design of ASP.NET Core. It is designed to be a lightweight, high-performance, and easy-to-use library for building web applications in C++.

> [!IMPORTANT]
> Currently, this library only targets Linux platforms. It is not the library you are looking for if you want cross-platform support.

---

# Getting Started

> [!NOTE]
> Before you move on, ensure you have CMake and your C++ compiler supports C++ 17.🫡

## Prepare the Repository

**minet-core** relies on some third-party libraries, you can clone it with the following command.

```bash
git clone https://github.com/Lord-Turmoil/minet-core.git --recurse-submodules
```

Or, you can clone it first, and then update the submodules.

```bash
git clone https://github.com/Lord-Turmoil/minet-core.git
cd minet-core
git submodule update --init --recursive
```

If you want to update the submodules, you can run the following command, or `./script/update.sh`.

```bash
git submodule update --recursive
```

## Your First Server

It is super easy to create a server with **minet-core**, just a few lines of code and you are ready to go.😆

```cpp
#include <minet/minet.h>

using namespace minet;

int main()
{
    WebHostBuilder::Create()
        ->UseAppSettings()
        ->Get("/ping", RequestHandler<>::Bind(
            [](const TextRequest& request, TextResponse& response) {
                response.Text().append("pong"); 
            }))
        ->Build()
        ->Run();
    return 0;
}
```

> [!CAUTION]
>
> For now, there is a known issue in this calling chain. You have to call `UseAppSettings()` (with or without parameter) before you call any other functions. It loads settings and initializes necessary components.😢

## The Demo

The example above might be a little too simple. For a more comprehensive demonstration, you can see the demo server under `demo/`. To see it live, use the following commands. It will run the demo server, and send some dummy requests to it.

```bash
# build and run the demo server
./script/demo.sh server

# in another terminal
./script/demo.sh client
```

> [!TIP]
>
> By default, the server launch at `http://localhost:5000`, so make sure this port isn't blocked or occupied. And the demo client uses `curl` to send requests.

---

# Endpoints

## Request/Response Wrapper

To relieve you from the tedious work of serializing and deserializing requests and responses, **minet-core** provides a wrapper for them. You can access the request and response via `TextRequest` and `TextResponse` for text-based content, or `JsonRequest` and `JsonResponse` for JSON content.

`TextRequest`/`TextResponse` provides `Text()` method to access the plain text body. `JsonRequest`/`JsonResponse` provides `Json()` method to access the JSON body. You can also use `Headers()` to access the headers, and `Params()` to access the query parameters.

## Request Handler

Request handler is where you write the server logic. It takes the request and response as parameters as shown below. `TRequest` and `TResponse` are template types for the wrapper.

```cpp
using RequestHandlerFn = std::function<void(const TRequest&, TResponse&)>;
```

To create a request handler, you need to use `RequestHandler<TRequest, TResponse>::Bind()`. By default, it will use `TextRequest` and `TextResponse`. Then, you can register endpoints like this.

```cpp
void ping(const TextRequest& request, TextResponse& response);
void echo(const TextRequest& request, JsonResponse& response);

// create WebHostBuilder

builder
    ->Get("/ping", RequestHandler<>::Bind(ping))
    ->Post("/echo", RequestHandler<TextRequest, JsonResponse>::Bind(echo));
```

See, isn't it easy?😉

---

# Configuration

**minet-core** provides flexible configurations for your server. By default, it will load configurations from `appsettings.json` in the working directory. You can change it by passing custom path to `UseAppSeettings()` function, just like what you see in the demo server.

The configuration file should be in JSON format, and comments are allowed. All settings in the configuration file are optional, so it can be as simple as `{}`. There are two built-in settings for **minet-core**, `server` and `logging`, and you can add extra settings.

## Settings

### Server

The server settings include the `name` of the server, and the `port` to listen on.

```json
{
    "server": {
        "name": "Basic",
        "port": 5000
    }
}
```

Notably, `name` specifies which server to use. Currently, **minet-core** has two server implementation.

- `Basic`: Default option, a blocking server that handles requests synchronously.
- `Mayhem`: An experimental server that handles requests asynchronously.

### Logging

**minet-core** uses`spdlog` for logging, and you can configure it in the `logging` section. The format of logging settings is as follows.

```json
{
    "logging": {
        "level": "Debug",
        "pattern": "",
        "sinks": [
            {
                "file": "stdout"
            },
            {
                "pattern": "[%Y-%m-%d %H:%M:%S] %8l [%6n]: %v",
                "file": "server.log"
            }
        ],
        "loggers": {
            "Demo": {
                "level": "Debug",
                "pattern": "%^[%Y-%m-%d %H:%M:%S] %l [%6n]: %v%$",
                "sinks": [
                    {
                        "file": "stdout"
                    },
                    {
                        "file": "demo.log"
                    }
                ]
            }
        }
    }
}
```

In the top level of `logging` are default configurations. And specific configurations for individual loggers are under `loggers`. Each logger has its own `level`, `pattern`, and `sinks`. If one of these are missing, it will inherit from the default settings.

`level` is the logging level, and it can be one of the following: `All`, `Fine`, `Debug`, `Info`, `Warning`, `Error`, `Critical`, `Disabled`.

`pattern` is the format of log messages. See [spdlog/Custom formatting](https://github.com/gabime/spdlog/wiki/3.-Custom-formatting#customizing-format-using-set_pattern) for more information. By default, it is empty, using the default pattern of `spdlog`. If it is missing, will inherit from the default one.

`sinks` is the output destination of the log. For `file` field, you can use `stdout` or `stderr` for console output, or specify a file path for file output. You can also specify `pattern` here. If pattern is missing, it will use the `pattern` in the current logger.

### Custom Settings

Of course, you can add other settings in the configuration file. In this case, you have to manually handle them in your code. To get the original configuration JSON, use `WebHostBuilder::GetAppSettings()`.

## Dependency Injection

> [!NOTE]
> **minet-core** uses [MinIoC](https://github.com/Lord-Turmoil/MinIoC) as the IoC container for dependency injection. See that repository for more information on how to use the container.

Behind the settings, **minet-core** uses dependency injection to manage all the pluggable components. You can get the IoC container via `WebHost::GetServiceContainer()`. It uses lazy initialization for singletons. Also, configurations are also injected into the container, you can get them by resolving `ServerConfig` and `LoggerConfig`.

If you want to add custom components, or replace the built-in ones, you can then register whatever you want after `UseAppSettings()`. The existing components will be replaced if you register the same interface again.

---

# Considerations

> [!NOTE]
>
> This section contains some personal design preferences.

## Heap vs Stack

You may notice that, in **minet-core**, some classes have private constructors, so that one can only create them in heap using the provided factory function. Why? Because I personally prefer using `->` in chained calls, and I don't want to mix `.` and `->` up. It eliminates the use of raw pointers, but also robs the freedom of choosing stack or heap.

---

# Acknowledgement

**minet-core** uses some open source libraries, thanks to their great work!🥰

- [nlohmann/json](https://github.com/nlohmann/json)
- [gabime/spdlog](https://github.com/gabime/spdlog)
- [Lord-Turmoil/MinIoC](https://github.com/Lord-Turmoil/MinIoC)
