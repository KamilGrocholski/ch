# HTTP server

<p>Building a simple HTTP server in C, inspired by modern web frameworks.</p>

## Source tree
```plaintext
├── main.c              # Entry file of the server
├── core                # Core utils for memory management, strings, arrays, etc.
│   ├── arena.c
│   ├── arena.h
│   ├── array.c
│   ├── array.h         # Dynamic arrays with metadata headers
│   ├── assert.h        # Custom asserts
│   ├── defines.h       # Common macro utilities and type definitions (e.g., u32, i32)
│   ├── hashmap.c
│   ├── hashmap.h
│   ├── logger.c
│   ├── logger.h        # Logger used throughout the whole project
│   ├── memory.c
│   ├── memory.h        # Memory management utilities
│   ├── str.c
│   ├── str.h           # String view
│   ├── strhashmap.c
│   ├── strhashmap.h    # Hashmap but for str_t only keys and values
│   ├── string.c
│   ├── string.h        # C strings with metadata headers
├── fs                  # Filesystem utils
│   ├── fs.c
│   └── fs.h
├── env                 # Env utils for loading and parsing .env like files
│   ├── env.c
│   └── env.h
└── net                 # Network utils
    ├── http
    │   ├── method.c
    │   ├── middleware.c
    │   ├── request.c
    │   ├── response.c
    │   ├── router.c
    │   └── status.c
    ├── http.c
    ├── http.h          # All in one HTTP header
    ├── url.c
    └── url.h
```

## How to setup

Go to `src/main.c` and add routes

## To start
```bash
make && ./bin/main
```

## To run unit tests
```bash
make test
```
