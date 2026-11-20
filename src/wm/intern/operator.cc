#include "wm/operator.hh"

#include <map>

namespace wm {

static std::map<std::string, OperatorType> &registry()
{
  static std::map<std::string, OperatorType> types;
  return types;
}

void register_operator(OperatorType type)
{
  std::string id = type.id;
  registry()[id] = std::move(type);
}

const OperatorType *find_operator(const std::string &id)
{
  auto it = registry().find(id);
  return it == registry().end() ? nullptr : &it->second;
}

}  // namespace wm
