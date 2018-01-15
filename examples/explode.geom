#version 330 core
#pragma qrk_include < core.glsl >
#pragma qrk_include < transforms.glsl >

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT {
  vec2 texCoords;
  vec3 fragPos;
  vec3 fragNormal;
}
gs_in[];

out VS_OUT {
  vec2 texCoords;
  vec3 fragPos;
  vec3 fragNormal;
}
gs_out;

void passThroughAttribs(int idx) {
  gs_out.texCoords = gs_in[idx].texCoords;
  gs_out.fragPos = gs_in[idx].fragPos;
  gs_out.fragNormal = gs_in[idx].fragNormal;
}

vec4 explode(vec4 position, vec3 normal) {
  float magnitude = 0.5;
  vec3 direction = normal * (sin(qrk_time) / 2.0 + 0.5) * magnitude;
  return position + vec4(direction, 0.0);
}

void main() {
  vec3 normal = qrk_calculateNormal(vec3(gl_in[0].gl_Position),
                                    vec3(gl_in[1].gl_Position),
                                    vec3(gl_in[2].gl_Position));

  gl_Position = explode(gl_in[0].gl_Position, normal);
  passThroughAttribs(0);
  EmitVertex();

  gl_Position = explode(gl_in[1].gl_Position, normal);
  passThroughAttribs(1);
  EmitVertex();

  gl_Position = explode(gl_in[2].gl_Position, normal);
  passThroughAttribs(2);
  EmitVertex();

  EndPrimitive();
}
