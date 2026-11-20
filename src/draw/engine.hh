#pragma once

/* A draw engine is one viewport mode (ray march, mesh). New modes are new registered engines. */

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "draw/view.hh"
#include "gpu/framebuffer.hh"
#include "scene/scene.hh"

namespace draw {

class Engine {
 public:
  virtual ~Engine() = default;
  virtual void sync(const scene::Scene &scene) = 0;                    /* Pick up changes. */
  virtual void draw(const View &view, gpu::Framebuffer &target) = 0; /* Draw the image. */
};

struct EngineType {
  std::string id;    /* "RAYMARCH" */
  std::string label; /* "Ray march": shown in the viewport mode switch. */
  std::function<std::unique_ptr<Engine>()> create;
};

void register_engine(EngineType type);
const std::vector<EngineType> &engine_types();
const EngineType *find_engine_type(const std::string &id);

/** Register the built-in engines. Called by app at startup. */
void register_engines();

}  // namespace draw
