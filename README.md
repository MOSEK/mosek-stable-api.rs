Rust bindings for dynamic MOSEK Core API 12.0. 

This library is intended for anyone who wishes to 
- Build and distribute a binary using MOSEK, and wants to allow MOSEK to be updated without rebuilding
- Allow using MOSEK functionality without requiring MOSEK to be present at build time, and only requiring MOSEK at
  runtime if functionality is actually used.

The MOSEK Core API is a stable API for MOSEK that guarantees binary compatibility between versions. Core API 12.0 is
based on functionality available in MOSEK 12.0 and will be supported for multiple subsequent versions. This crate does
*not* include or install the actual MOSEK Core API 12.0 library, but loads it dynamically on demand at runtime. This
means that an application using this crate *does not* require the MOSEK Core API 12.0 library to be present at build or
run time until loading is explicitly requested.

# Usage

Before any API function can be used, the library must be initialized. This is done with one of the following functions:
- `initialize()` attempt to load MOSEK Core API library from default system path,
- `initialize_with_paths(paths : &[&Path])` which provide a list of search paths for the MOSEK Core API library, and
- `initialize_with_defaults()` which will look for the MOSEK Core API library in a number of standard installation locations.

When the library is successfully initialized, all subsequent calls to either initialization function will do nothing and succeed.

A few examples are included under `examples/`. To test, the MOSEK Core API library must be discoverable, which means either in a standard installation location, in the system library path or in the loader path variable:
- `PATH` environment variable on Windows
- `LD_LIBRARY_PATH` on Linux
- `DYLD_LIBRARY_PATH` on OS X

# Default locations

Default installation locations (location of the `mosek/` folder) are:
- `$HOME/` (Linux and Mac OS X) 
- `$HOME/.local/` (Linux and Mac OS X) 
- `$HOME/Applications/` (Mac OS X) 
- `$HOME/Applications/` (Windows) 
- `$LOCALAPPDATA/` (Windows)
