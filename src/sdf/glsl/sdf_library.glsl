// SDF formulas, shared by the GPU (GLSL 3.30) and the CPU (C++ with GLM, see intern/sdf.cc).
//
// Rules so this file compiles as both:
//  - only use names that exist in GLSL and in GLM: vec3, length, abs, min, max, ...
//  - write float numbers with an f: 0.0f (GLSL 3.30 accepts it too).
//
// Formulas from Inigo Quilez, https://iquilezles.org/articles/distfunctions/

// Sphere at the origin.
float sd_sphere(vec3 p, float radius)
{
  return length(p) - radius;
}

// Box at the origin. `half_size` is half its width, height and depth.
float sd_box(vec3 p, vec3 half_size)
{
  vec3 q = abs(p) - half_size;
  return length(max(q, vec3(0.0f))) + min(max(q.x, max(q.y, q.z)), 0.0f);
}

// TODO(roadmap 5): sd_primitive(type, p, params): the switch that picks the formula by PrimType,
//                  shared by the CPU and the ray march shader.
// TODO(later): combine operators (smooth union, subtract, intersect), more shapes (torus, cone).
