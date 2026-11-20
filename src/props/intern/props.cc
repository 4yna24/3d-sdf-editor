#include "props/props.hh"

namespace props {

static PropertyDesc &add(StructDesc &s, std::string id, std::string label, Type type, Value def)
{
  PropertyDesc &p = s.props.emplace_back();
  p.id = std::move(id);
  p.label = std::move(label);
  p.type = type;
  p.default_value = std::move(def);
  return p;
}

PropertyDesc &StructDesc::add_float(std::string id, std::string label, float default_value)
{
  return add(*this, std::move(id), std::move(label), Type::Float, default_value);
}

PropertyDesc &StructDesc::add_int(std::string id, std::string label, int default_value)
{
  return add(*this, std::move(id), std::move(label), Type::Int, default_value);
}

PropertyDesc &StructDesc::add_bool(std::string id, std::string label, bool default_value)
{
  return add(*this, std::move(id), std::move(label), Type::Bool, default_value);
}

PropertyDesc &StructDesc::add_string(std::string id, std::string label, std::string default_value)
{
  return add(*this, std::move(id), std::move(label), Type::String, std::move(default_value));
}

const PropertyDesc *StructDesc::find(const std::string &id) const
{
  for (const PropertyDesc &p : props) {
    if (p.id == id) {
      return &p;
    }
  }
  return nullptr;
}

}  // namespace props
