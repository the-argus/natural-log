#include "natural_log.hpp"
#include <array>
#include <atomic>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <mutex>
#ifdef NATURAL_LOG_RAYLIB
#include <raylib.h>
#endif

static void internal(int msgType, const char *text,
                     va_list args) NATURAL_LOG_NOEXCEPT;
static std::atomic<ln::level_e> minLevel;

// make sure raylib matches up with our types so we can convert safely
// clang-format off
#ifdef NATURAL_LOG_RAYLIB
static_assert(std::underlying_type_t<ln::level_e>(ln::level_e::ALL) == static_cast<int>(::LOG_ALL));
static_assert(std::underlying_type_t<ln::level_e>(ln::level_e::TRACE) == static_cast<int>(::LOG_TRACE));
static_assert(std::underlying_type_t<ln::level_e>(ln::level_e::DEBUG) == static_cast<int>(::LOG_DEBUG));
static_assert(std::underlying_type_t<ln::level_e>(ln::level_e::INFO) == static_cast<int>(::LOG_INFO));
static_assert(std::underlying_type_t<ln::level_e>(ln::level_e::WARNING) == static_cast<int>(::LOG_WARNING));
static_assert(std::underlying_type_t<ln::level_e>(ln::level_e::ERROR) == static_cast<int>(::LOG_ERROR));
static_assert(std::underlying_type_t<ln::level_e>(ln::level_e::FATAL) == static_cast<int>(::LOG_FATAL));
static_assert(std::underlying_type_t<ln::level_e>(ln::level_e::NONE) == static_cast<int>(::LOG_NONE));
#endif
// clang-format on

namespace ln {

// tell raylib to use our internal colored logger
void init() NATURAL_LOG_NOEXCEPT
{
    minLevel = ln::level_e::ALL;
#ifdef NATURAL_LOG_RAYLIB
    SetTraceLogCallback(internal);
#endif
}

void set_minimum_level(level_e level) NATURAL_LOG_NOEXCEPT
{
#ifdef NATURAL_LOG_RAYLIB
    SetTraceLogLevel(std::underlying_type_t<level_e>(level));
#endif
    minLevel = level;
}

void log(level_e level, fmt::string_view message) NATURAL_LOG_NOEXCEPT
{
    static std::mutex logmutex;
    std::lock_guard lock(logmutex);
    if (level <= minLevel)
        return;

    {
        std::time_t currenttime = std::time(nullptr);
        fmt::print("[{:%H:%M:%S}] ", fmt::localtime(currenttime));
    }

    switch (level) {
    case level_e::INFO:
        fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::white),
                   "[INFO] : ");
        break;
    case level_e::ERROR:
        fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::red),
                   "[ERROR] : ");
        break;
    case level_e::WARNING:
        fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::yellow),
                   "[WARN] : ");
        break;
    case level_e::DEBUG:
        fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::green),
                   "[DEBUG] : ");
        break;
    case level_e::FATAL:
        fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::red),
                   "[FATAL] : ");
        break;
    case level_e::TRACE:
        fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::cyan),
                   "[TRACE] : ");
        break;
    case level_e::NONE:
        fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::yellow),
                   "[UNDEFINED NONELEVEL] : ");
        break;
    default:
        fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::yellow),
                   "[WARN] : (Unimplemented log level used!) ");
        break;
    }

    // use cout because its a string view and printf doesnt know how to do that
    fmt::println("{}", message);
}
} // namespace ln

#ifdef NATURAL_LOG_RAYLIB
static void internal(int msgType, const char *text,
                     va_list args) NATURAL_LOG_NOEXCEPT
{
    std::array<char, 512> buffer;
    int bytesPrinted = vsnprintf(buffer.data(), buffer.size(), text, args);
    if (bytesPrinted < 0) {
        ln::log(ln::level_e::WARNING,
                "encoding error occured when trying to print string");
        return;
    }

    if (bytesPrinted > buffer.size())
        log(ln::level_e::WARNING,
            "Failed to completely print the following message:");

    ln::log(static_cast<ln::level_e>(msgType), buffer.data());
}
#endif
