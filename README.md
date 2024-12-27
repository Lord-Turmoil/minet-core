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

## Features

Although **minet core** is simple, it indeed has some cool features.😎

- **Easy to Use**: It is super easy to create a server with just a few lines of code.
- **Flexible Configuration**: You can configure the server with a JSON file.
- **Dependency Injection**: It uses IoC to manage components, and you can replace them with your own.
- **Multiple Servers**: It provides multiple server implementations, good for study and comparison.
- **Extensible**: It is designed to be extensible, and you can build your framework on top of it.

## Purpose

The main purpose for **minet core** is to learn the mechanism behind an HTTP server. The key is to understand how the server accepts and handles requests, and how it sends responses back. It is also a good practice to learn how to design a library that is easy to use and flexible to extend.

## Key Points

The most important part of **minet core** is the design of the server. The server is responsible for accepting requests. As my knowledge expands, I have implemented three different servers.

The first is `Basic` server, accepting requests in one thread. It is simple and easy to understand, but it is not efficient.

The second is `Threaded` server, which handles requests in multiple threads with a thread pool.
It is way more efficient than the `Basic` server, but it is not the best.

The third is `Mayhem` server, based on `Threaded` server, but also utilizes epoll for asynchronous I/O. It is the most efficient server in **minet core**.

---

# Getting Started

> [!NOTE]
> Before you move on, ensure you have CMake and your C++ compiler supports C++ 17.🫡

> [!WARNING]
> If you build **minet-core** with TSan (Thread Sanitizer) on Ubuntu 20.04, you may get linker error saying missing `libtsan_preinit.o`. Check [(TSAN) /usr/bin/ld can't find libtsan_preinit.o](https://stackoverflow.com/questions/77858687/tsan-usr-bin-ld-cant-find-libtsan-preinit-o) for solutions.

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

## Build the Library

> [!NOTE]
> By default, the sanitizer options are OFF. You can enable address sanitizer or thread sanitizer by enabling `MINET_ASAN` or `MINET_TSAN` option respectively. Since these two sanitizers are not compatible with each other, TSan will be ignored if ASan is enabled.

### Have a try

Before you decide to use **minet-core** in your project, you can build the demo server to see how it works. The following commands will build libray and the demo server. You can also jump to the following section to see the bundled demo.

```bash
./script/build.sh debug   # build the debug version
./script/build.sh release # build the release version
```

### Use it in your project

To use **minet core** in your project, simply add it as a subdirectory in your CMake project. Then link your target with `minetcore`.

```cmake
add_subdirectory("minet-core")
target_link_libraries(your-app-name PRIVATE minetcore)
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
        ->Get("/ping", RequestHandler::Bind(
            [](const TextRequest& request, TextResponse& response) {
                response.Text().assign("pong"); 
            }))
        ->Build()
        ->Run();
    return 0;
}
```

> [!CAUTION]
>For now, there is a known issue in this calling chain. You have to call `UseAppSettings()` (with or without parameter) before you call any other functions. It loads settings and initializes necessary components.😢

## The Demo

The example above might be a little too simple. For a more comprehensive demonstration, you can see the demo server under `demo/`. To see it live, use the following commands. It will run the demo server, and send some dummy requests to it. There are two server options available. And the client will launch 4 processes to send requests at the same time.

```bash
# build and run the demo server
./script/demo.sh server        # run with Basic server
./script/demo.sh server mayhem # run with Mayhem server

# in another terminal
./script/demo.sh client        # 4 processes, each sending 10 requests
./script/demo.sh client N      # 4 processes, each sending N requests
./script/demo.sh client N M    # 2^M processes, each sending N requests
```

Basic server handles requests in one thread, so you'll see the client return one response at a time. Mayhem server handles requests asynchronously, so you'll see a significant speedup in the client.

The configuration files are also provided in `demo/`, you can modify them to see the effects.

> [!TIP]
>By default, the server launch at `http://localhost:5000`, so make sure this port isn't blocked or occupied. And the demo client uses `curl` to send requests.

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

**minet-core** provides a convenient way to create request handlers by using predefined templates. For plain text request and response, you can use `RequestHandler` directly. For JSON request and response, you can use `RestfulHandler`. If request and response types are different, you can use `CustomHandler` instead. Below are their definitions.

```cpp
using RequestHandler = RequestHandlerImpl<TextRequest, TextResponse>;
using RestfulHandler = RequestHandlerImpl<JsonRequest, JsonResponse>;
template <typename TRequest, typename TResponse>
using CustomHandler = RequestHandlerImpl<TRequest, TResponse>;
```

```cpp
void text(const TextRequest& request, TextResponse& response);
void json(const JsonRequest& request, JsonResponse& response);
void custom(const TextRequest& request, JsonResponse& response);

// create WebHostBuilder

builder
    ->Get("/text", RequestHandler::Bind(text))
    ->Post("/json", RestfulHandler::Bind(json))
    ->Post("/custom", CustomHandler::Bind(custom));
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
        "threads": 4,
        "capacity": 1024,
        "port": 5000
    }
}
```

Notably, `name` specifies which server to use. Currently, **minet-core** has three server implementation.

- `Basic`: Default option, a blocking server that handles requests synchronously.
- `Threaded`: A server that handles requests in multiple threads with a thread pool.
- `Mayhem`: An experimental server that handles requests using both epoll and thread pool.

If you choose to use `Basic` server, then `threads` and `capacity` are ignored. For `Threaded` and `Mayhem` server, `threads` is the number of worker threads, and `capacity` is the maximum requests queued on each worker thread.

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
>This section contains some personal design preferences.

## Heap vs Stack

You may notice that, in **minet-core**, some classes have private constructors, so that one can only create them in heap using the provided factory function. Why? Because I personally prefer using `->` in chained calls, and I don't want to mix `.` and `->` up. It eliminates the use of raw pointers, but also robs the freedom of choosing stack or heap.

---

# Acknowledgement

**minet-core** uses some open source libraries, thanks to their great work!🥰

- [nlohmann/json](https://github.com/nlohmann/json) v3.11.3
- [gabime/spdlog](https://github.com/gabime/spdlog) v1.15.0
- [Lord-Turmoil/MinIoC](https://github.com/Lord-Turmoil/MinIoC) v1.3.0
