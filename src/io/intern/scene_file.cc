#include "io/io.hh"

#include "base/log.hh"

namespace io {

bool save_scene(const scene::Document & /*doc*/, const std::string &path)
{
  base::log_warning("Saving is not written yet ({})", path);
  return false;
}

bool load_scene(scene::Document & /*doc*/, const std::string &path)
{
  base::log_warning("Loading is not written yet ({})", path);
  return false;
}

}  // namespace io
