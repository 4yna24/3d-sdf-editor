#pragma once

/* Worker threads for long jobs: mesher, CPU renderer, export.
 * A job gets a copy of what it needs, never the live scene. See ai/code-structure.md, "Threads".
 *
 * TODO(roadmap 7): implement when the mesher needs it. */

namespace base {
class ThreadPool;
}
