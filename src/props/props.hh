#pragma once

/* Describe a value once (name, type, range). The Properties editor, Preferences, save/load,
 * undo and operator inputs all read that description. See ai/scene.md, "Property descriptions".
 *
 * TODO(roadmap 3): paths ("objects/42/shape/radius") and get/set by path, used by "props.set". */

#include <map>
#include <string>
#include <variant>
#include <vector>

#include "base/math.hh"

namespace props {

enum class Type { Bool, Int, Float, Vec3, Color, Enum, String };
enum class Unit { None, Length, Angle };

using Value = std::variant<bool, int, float, base::vec3, std::string>;

/** One property: what it is called, its type, its limits. */
struct PropertyDesc {
  std::string id;      /* "radius": used in paths and files. */
  std::string label;   /* "Radius": shown in the UI. */
  std::string tooltip;
  Type type = Type::Float;
  Value default_value;
  float min = -1e9f, max = 1e9f;
  Unit unit = Unit::None;

  PropertyDesc &range(float lo, float hi)
  {
    min = lo;
    max = hi;
    return *this;
  }
  PropertyDesc &set_unit(Unit u)
  {
    unit = u;
    return *this;
  }
  PropertyDesc &set_tooltip(std::string text)
  {
    tooltip = std::move(text);
    return *this;
  }
};

/** The list of properties of one type (a shape type, the preferences, an operator's inputs). */
struct StructDesc {
  std::vector<PropertyDesc> props;

  PropertyDesc &add_float(std::string id, std::string label, float default_value);
  PropertyDesc &add_int(std::string id, std::string label, int default_value);
  PropertyDesc &add_bool(std::string id, std::string label, bool default_value);
  PropertyDesc &add_string(std::string id, std::string label, std::string default_value = {});
  const PropertyDesc *find(const std::string &id) const;
};

/** Named values, e.g. the inputs an operator runs with. */
class Values {
 public:
  void set(const std::string &id, Value value)
  {
    values_[id] = std::move(value);
  }
  bool has(const std::string &id) const
  {
    return values_.contains(id);
  }
  /** The value, or `fallback` when it is missing or has another type. */
  template<class T> T get(const std::string &id, T fallback = {}) const
  {
    auto it = values_.find(id);
    if (it == values_.end()) {
      return fallback;
    }
    const T *v = std::get_if<T>(&it->second);
    return v ? *v : fallback;
  }

 private:
  std::map<std::string, Value> values_;
};

}  // namespace props
