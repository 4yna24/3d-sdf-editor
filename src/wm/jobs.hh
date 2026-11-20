#pragma once

/* Background jobs (mesher, export). Finished results are picked up by the main loop, which then
 * sends a notifier. See ai/code-structure.md, "Threads".
 * TODO(roadmap 7): uses base::ThreadPool. */

namespace wm {

class Jobs {
 public:
  /** Called once per main loop pass. Returns true when a job finished. */
  bool collect_finished()
  {
    return false;
  }
};

}  // namespace wm
