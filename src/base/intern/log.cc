#include "base/log.hh"

#include <cstdio>

namespace base {

void log_write(LogLevel level, std::string_view message)
{
  const char *prefix = level == LogLevel::Error   ? "error"
                       : level == LogLevel::Warning ? "warning"
                                                    : "info";
  std::fprintf(stderr, "[%s] %.*s\n", prefix, int(message.size()), message.data());
}

}  // namespace base
