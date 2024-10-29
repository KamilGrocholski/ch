# HTTP server

<p>Building a simple HTTP server written in C, designed to look like today's web frameworks.</p>

## Source tree
├── main.c              # Entry file of the server
├── core                # Core utils memory management, strings, arrays, etc.
│   ├── arena.c
│   ├── arena.h
│   ├── array.c
│   ├── array.h         # Dynamic arrays with metadata headers
│   ├── assert.h        # Custom asserts
│   ├── defines.h       # Common macro utils and types like u32, i32...
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
│   ├── string.h        # C strings with metadata header
├── fs                  # Filesystem utils
│   ├── fs.c
│   └── fs.h
└── net                 # Network utils
    ├── http
    │   ├── method.c
    │   ├── request.c
    │   └── router.c
    ├── http.c
    ├── http.h          # All in one HTTP header
    ├── url.c
    └── url.h

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
