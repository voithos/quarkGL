#ifndef QUARKGL_DEBUG_GEOM_
#define QUARKGL_DEBUG_GEOM_

const float DEFAULT_MAGNITUDE = 0.1;

/**
 * Generates a normal line for visualization purposes. Requires
 * line_strip output.
 */
void qrk_generateNormalLine(vec4 position, vec3 normal, float magnitude) {
  gl_Position = position;
  EmitVertex();
  gl_Position = position + vec4(normal, 0.0) * magnitude;
  EmitVertex();
  EndPrimitive();
}

/**
 * Generates a normal line for visualization purposes. Requires
 * line_strip output.
 */
void qrk_generateNormalLine(vec4 position, vec3 normal) {
  qrk_generateNormalLine(position, normal, DEFAULT_MAGNITUDE);
}

#endif
