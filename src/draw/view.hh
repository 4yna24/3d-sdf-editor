#pragma once

#include "base/math.hh"

namespace draw {

/** What a draw engine needs from a viewport. The viewport camera makes it. */
struct View {
  base::mat4 view{1.0f};
  base::mat4 projection{1.0f};
  int width = 0;
  int height = 0;
};

}  // namespace draw
