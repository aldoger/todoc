# TODO List API in C

A small HTTP server written in C that serves a fixed list of todos as JSON.

## Requirements

- CMake 3.23 or newer
- A C99-compatible compiler
- `json-c`
- A Unix-like environment with POSIX sockets

On Debian or Ubuntu, install the build dependencies with:

```bash
sudo apt install build-essential cmake pkg-config libjson-c-dev
```

## Build

```bash
cmake -S . -B build
cmake --build build
```

The executable is created at `build/Server`.

## Run

```bash
./build/Server
```

The server listens on port `8080`.

## API

### Get todos

```http
GET /todos HTTP/1.1
Host: localhost:8080
```

Example:

```bash
curl http://localhost:8080/todos
```

Example response:

```json
{
  "status": 200,
  "message": "Todos retrieved successfully",
  "data": [
    {
      "priority": 1,
      "desc": "wash dishes"
    },
    {
      "priority": 2,
      "desc": "clean bedroom"
    }
  ]
}
```