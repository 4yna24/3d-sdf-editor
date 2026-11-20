#pragma once

/* User preferences. Saved in the user config folder, not in the scene file.
 * See ai/ui.md, "Preferences". */

#include "props/props.hh"

namespace prefs {

struct Preferences {
  float ui_scale = 1.0f;
  /* TODO: theme, mesh resolution, ray march quality, orbit style, GPU or CPU renderer. */
};

/** The property list of Preferences. The Preferences editor and save/load read it. */
const props::StructDesc &describe();

/** TODO(roadmap 8): read/write ~/.config/sdf-editor/preferences.json. */
bool load(Preferences &prefs);
bool save(const Preferences &prefs);

}  // namespace prefs
