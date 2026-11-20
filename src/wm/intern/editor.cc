#include "wm/editor.hh"

namespace wm {

static std::vector<EditorType> &registry()
{
  static std::vector<EditorType> types;
  return types;
}

void register_editor_type(EditorType type)
{
  registry().push_back(std::move(type));
}

const EditorType *find_editor_type(const std::string &id)
{
  for (const EditorType &t : registry()) {
    if (t.id == id) {
      return &t;
    }
  }
  return nullptr;
}

const std::vector<EditorType> &editor_types()
{
  return registry();
}

}  // namespace wm
