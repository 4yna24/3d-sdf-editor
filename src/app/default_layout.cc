#include "app/app.hh"

namespace app {

std::unique_ptr<wm::Workspace> default_layout()
{
  using wm::SplitDir;
  auto ws = std::make_unique<wm::Workspace>();
  ws->name = "Layout";
  ws->screen.root = wm::make_split(
      SplitDir::LeftRight,
      0.72f,
      wm::make_area("VIEW_3D"),
      wm::make_split(
          SplitDir::TopBottom, 0.4f, wm::make_area("OUTLINER"), wm::make_area("PROPERTIES")));
  return ws;
}

}  // namespace app
