#pragma once

#include <format>
#include <string_view>

namespace base {

enum class LogLevel { Info, Warning, Error };

void log_write(LogLevel level, std::string_view message);

template<class... Args> void log_info(std::format_string<Args...> fmt, Args &&...args)
{
  log_write(LogLevel::Info, std::format(fmt, std::forward<Args>(args)...));
}

template<class... Args> void log_warning(std::format_string<Args...> fmt, Args &&...args)
{
  log_write(LogLevel::Warning, std::format(fmt, std::forward<Args>(args)...));
}

template<class... Args> void log_error(std::format_string<Args...> fmt, Args &&...args)
{
  log_write(LogLevel::Error, std::format(fmt, std::forward<Args>(args)...));
}

}  // namespace base
