Rust bindings for dynamic MOSEK Core API 12.0. 

The MOSEK Core API is a stable API for MOSEK that guarantees binary
compatibility between versions. Core API 12.0 is based on functionality
available in MOSEK 12.0 and will be supported for multiple subsequent versions.
This crate does *not* include the actual MOSEK Core API 12.0 library, but loads
it dynamically on demand at runtime. This means that an application using this
crate *does not* require the MOSEK Core API 12.0 library to be present at build
or run time until loading is explicitly requested.

# Usage

Calling `initialize()` or `initialize_with_paths()` successfully will return a struct with access to all API functions.

A few examples are included. To test, the MOSEK Core API library must be discoverable, which means that the path to the
library must be in the
- `PATH` environment variable on Windows
- `LD_LIBRARY_PATH` on Linux
- `DYLD_LIBRARY_PATH` on OS X
