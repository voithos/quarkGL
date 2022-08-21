#version 460 core
#pragma qrk_include < debug.geom>

// An example geometry shader that generates vertices along the normal lines.

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in VS_OUT {
  vec2 texCoords;
  vec3 fragPos;
  vec3 fragNormal;
}
gs_in[];

uniform mat4 projection;

/** Transform normals from view-space to clip-space. */
vec3 projectNormal(vec3 viewSpaceNormal) {
  return normalize(vec3(projection * vec4(viewSpaceNormal, 0.0)));
}

void main() {
  qrk_generateNormalLine(gl_in[0].gl_Position,
                         projectNormal(gs_in[0].fragNormal), 0.05);
  qrk_generateNormalLine(gl_in[1].gl_Position,
                         projectNormal(gs_in[1].fragNormal), 0.05);
  qrk_generateNormalLine(gl_in[2].gl_Position,
                         projectNormal(gs_in[2].fragNormal), 0.05);
}
