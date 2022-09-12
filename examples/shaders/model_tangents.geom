#version 460 core
#pragma qrk_include < debug.geom>

// An example geometry shader that generates vertices along the normal and
// tangent lines.

layout(triangles) in;
layout(line_strip, max_vertices = 12) out;

in VS_OUT {
  vec2 texCoords;
  vec3 fragPos_viewSpace;
  vec3 fragNormal_viewSpace;
  vec3 fragTangent_viewSpace;
  mat3 fragTBN_viewSpace;  // Transforms from tangent frame to view frame.
}
gs_in[];

uniform mat4 projection;

/** Transform normals from view-space to clip-space. */
vec3 projectNormal(vec3 viewSpaceNormal) {
  return normalize(vec3(projection * vec4(viewSpaceNormal, 0.0)));
}

void main() {
  qrk_generateNormalLine(gl_in[0].gl_Position,
                         projectNormal(gs_in[0].fragNormal_viewSpace), 0.25);
  qrk_generateNormalLine(gl_in[1].gl_Position,
                         projectNormal(gs_in[1].fragNormal_viewSpace), 0.25);
  qrk_generateNormalLine(gl_in[2].gl_Position,
                         projectNormal(gs_in[2].fragNormal_viewSpace), 0.25);

  qrk_generateNormalLine(gl_in[0].gl_Position,
                         projectNormal(gs_in[0].fragTangent_viewSpace), 0.25);
  qrk_generateNormalLine(gl_in[1].gl_Position,
                         projectNormal(gs_in[1].fragTangent_viewSpace), 0.25);
  qrk_generateNormalLine(gl_in[2].gl_Position,
                         projectNormal(gs_in[2].fragTangent_viewSpace), 0.25);
}
