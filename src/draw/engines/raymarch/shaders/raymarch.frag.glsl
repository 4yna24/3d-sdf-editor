#version 330 core
// Ray march fragment shader: one ray per pixel through the SDF program.
// TODO(roadmap 5): include sdf/glsl/sdf_library.glsl, loop over the primitives buffer, write depth.

out vec4 frag_color;

void main()
{
  frag_color = vec4(0.0);
}
