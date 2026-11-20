#include "prefs/prefs.hh"

namespace prefs {

const props::StructDesc &describe()
{
  static const props::StructDesc desc = [] {
    props::StructDesc d;
    d.add_float("ui_scale", "Resolution Scale", 1.0f).range(0.5f, 3.0f);
    return d;
  }();
  return desc;
}

bool load(Preferences & /*prefs*/)
{
  return false; /* TODO(roadmap 8) */
}

bool save(const Preferences & /*prefs*/)
{
  return false; /* TODO(roadmap 8) */
}

}  // namespace prefs
