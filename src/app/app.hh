#pragma once

#include <memory>

#include "wm/screen.hh"

namespace app {

/** Every module's register function, in one place. */
void register_all();

/** The "Layout" workspace: 3D Viewport on the left, Outliner over Properties on the right. */
std::unique_ptr<wm::Workspace> default_layout();

}  // namespace app
