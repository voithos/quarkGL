#version 460 core
layout(location = 0) in vec3 vertexPos;

out vec3 skyboxCoords;

// Note that the view matrix should *not* have any translation, since the skybox
// is always rendered at the camera origin and thus should "follow" the camera.
uniform mat4 view;
uniform mat4 projection;

void main() {
  // No model transform needed for a skybox.
  vec4 pos = projection * view * vec4(vertexPos, 1.0);
  // The skybox is meant to be drawn last, so to take advantage of early depth
  // testing, we set the vertex's z component to w so that after the perspective
  // division by w the resulting normalized device coordinate will equal 1.0,
  // which is the maximum depth value. This allows the skybox to be rendered
  // behind everything else.
  gl_Position = pos.xyww;
  // The sample coordinates are equivalent to the interpolated vertex positions.
  skyboxCoords = vertexPos;
}