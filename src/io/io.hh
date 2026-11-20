#pragma once

/* Scene files and export. See ai/scene.md, "Save / load" and "Export".
 * Export formats will be one file each: io/export_obj.cc, io/export_stl.cc, io/export_png.cc. */

#include <string>

#include "scene/document.hh"

namespace io {

/** TODO(roadmap 9): one JSON file with "version": 1, written from the property descriptions. */
bool save_scene(const scene::Document &doc, const std::string &path);
bool load_scene(scene::Document &doc, const std::string &path);

}  // namespace io
