# natural_log

A really simple c++20 logger, with optional raylib integration. Uses zig as its
buildsystem.

## API

This is all you are getting with `#include <natural_log.hpp>`, give or take an
enum and some macros:

```cpp
#include <fmt/format.h>

/// Initialize any static resources related to the logger. should be called
/// before performing any logging operations.
void ln::init();

/// Set level of urgency below which log messages will not be printed.
void ln::set_minimum_level(level_e level);

/// Log a message with a given urgency.
void ln::log(level_e level, fmt::string_view message);

/// Log a message with a given urgency while also performing a string format.
/// Prints into a stack buffer. Template argument buffersize determines the max
/// bytes of that buffer. If it overwrites the buffer, the message will be
/// truncated.
template <typename... T, size_t buffersize = 512>
inline void ln::log_fmt(
    level_e level,
    fmt::format_string<T...> fmt_string,
    T &&...args);
```

## Usage

In practice, you will be using macros to log. The macros are as follows:

- `LN_FATAL`
- `LN_ERROR`
- `LN_WARN`
- `LN_INFO`
- `LN_DEBUG`
- `LN_TRACE`
- `LN_FATAL_FMT`
- `LN_ERROR_FMT`
- `LN_WARN_FMT`
- `LN_INFO_FMT`
- `LN_DEBUG_FMT`
- `LN_TRACE_FMT`

Macros such as `LN_INFO` will just log a simple compile time known string. For
example:

```cpp
#include <natural_log.hpp>
#include "myheader.h"

int main() {
    // initialize logger
    ln::init();
    LN_INFO("Beginning initialization...");
    myinit();
    LN_INFO("Exiting...");
}
```

And the variants with `_FMT` at the end allow you to pass in arguments to a
compile time known format string. For example:

```cpp
#include <natural_log.hpp>

float checked_divide(float numerator, float denominator) {
    if (denominator == 0) {
        LN_FATAL_FMT("Attempt to divide {} by 0. Aborting program", numerator);
    }
    return numerator / denominator;
}
```
