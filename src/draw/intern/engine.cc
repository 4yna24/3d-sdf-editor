#include "draw/engine.hh"

#include "draw/engines/mesh/mesh_engine.hh"
#include "draw/engines/raymarch/raymarch_engine.hh"

namespace draw {

static std::vector<EngineType> &registry()
{
  static std::vector<EngineType> types;
  return types;
}

void register_engine(EngineType type)
{
  registry().push_back(std::move(type));
}

const std::vector<EngineType> &engine_types()
{
  return registry();
}

const EngineType *find_engine_type(const std::string &id)
{
  for (const EngineType &t : registry()) {
    if (t.id == id) {
      return &t;
    }
  }
  return nullptr;
}

void register_engines()
{
  register_raymarch_engine();
  register_mesh_engine();
}

}  // namespace draw
