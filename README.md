# C++ HTTP Server

A multi-threaded HTTP/1.1 server built from scratch in C++17 using Winsock.

## Features

- **Concurrent connections** - Thread pool handles multiple clients simultaneously
- **HTTP/1.1 parsing** - Parses request line, headers, and body
- **Static file serving** - Serves HTML, CSS, JS, images with correct MIME types
- **Security** - Path traversal attack prevention via `std::filesystem` sanitization
- **Modern C++** - RAII, move semantics, smart pointers, C++17 features

## Architecture

```
┌─────────────┐     ┌──────────────┐     ┌─────────────────┐
│   Client    │────►│   Server     │────►│   Thread Pool   │
│  (Browser)  │     │  (accept)    │     │   (N workers)   │
└─────────────┘     └──────────────┘     └─────────────────┘
                                                 │
                          ┌──────────────────────┼──────────────────────┐
                          ▼                      ▼                      ▼
                   ┌─────────────┐       ┌─────────────┐       ┌─────────────┐
                   │   Request   │       │   Response  │       │   File      │
                   │   Parser    │       │   Builder   │       │   Reader    │
                   └─────────────┘       └─────────────┘       └─────────────┘
```

## Project Structure

```
http-server/
├── include/
│   ├── socket.hpp         # RAII socket wrappers
│   ├── request.hpp        # HTTP request parser
│   ├── response.hpp       # HTTP response builder
│   ├── server.hpp         # Server class
│   └── thread_pool.hpp    # Thread pool implementation
├── src/
│   ├── main.cpp
│   ├── socket.cpp
│   ├── request.cpp
│   ├── response.cpp
│   ├── server.cpp
│   └── thread_pool.cpp
├── www/                   # Static files to serve
│   └── index.html
├── CMakeLists.txt
└── README.md
```

## Build

### Prerequisites
- C++17 compiler
- CMake 3.16+
- Windows (uses Winsock)

### Commands
```bash
cd http-server
mkdir build && cd build
cmake ..
cmake --build .
```

## Run

```bash
./Debug/http_server.exe
```

Server starts on `http://localhost:8080`

## Usage

```bash
# GET request
curl http://localhost:8080/index.html

# 404 Not Found
curl http://localhost:8080/nonexistent.html

# 403 Forbidden (path traversal blocked)
curl http://localhost:8080/../secret.txt

# 405 Method Not Allowed
curl -X POST http://localhost:8080/index.html
```

## Tech Stack

| Category | Technologies |
|----------|--------------|
| Language | C++17 |
| Build | CMake |
| Networking | Winsock (Windows sockets API) |
| Concurrency | `std::thread`, `std::mutex`, `std::condition_variable` |
| Filesystem | `std::filesystem` (C++17) |

## Key Concepts Demonstrated

- **RAII (Resource Acquisition Is Initialization)** - Socket wrappers automatically clean up resources
- **Move Semantics** - Efficient transfer of socket ownership between threads
- **Thread Pool Pattern** - Producer-consumer queue with condition variables
- **HTTP Protocol** - Request/response parsing and building
- **Security** - Input validation, path canonicalization

## HTTP Status Codes Supported

| Code | Reason | When Used |
|------|--------|-----------|
| 200 | OK | Successful GET request |
| 400 | Bad Request | Malformed HTTP request |
| 403 | Forbidden | Path traversal attempt |
| 404 | Not Found | File doesn't exist |
| 405 | Method Not Allowed | Non-GET method |

## Future Improvements

- [ ] HTTP POST support
- [ ] Graceful shutdown (signal handling)
- [ ] Config file for port/thread count
- [ ] Unit tests

## License

MIT
