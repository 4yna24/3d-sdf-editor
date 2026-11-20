#pragma once

#include <cstdint>

namespace base {

/** Ids never change and are never reused. Long-lived links use ids, never pointers. */
using Id = uint32_t;
inline constexpr Id NO_ID = 0;

}  // namespace base
