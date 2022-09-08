#version 460 core
#pragma qrk_include < core.glsl>
#pragma qrk_include < standard_lights.frag>
#pragma qrk_include < depth.frag>

// An example fragment shader that uses normal mapping.

in VS_OUT {
  vec2 texCoords;
  vec3 fragPos;
  vec3 fragNormal;
  vec3 fragTangent;
  mat3 fragTBN;  // Tangent, bitangent, normal frame.
}
fs_in;

out vec4 fragColor;

uniform QrkMaterial material;
uniform sampler2D normalMap;
uniform bool useVertexNormals;
uniform mat4 model;
uniform mat4 view;

void main() {
  vec3 normal;
  if (useVertexNormals) {
    normal = normalize(fs_in.fragNormal);
    // TODO: Temporarily rendering the normal for debugging.
    fragColor.rgb = (normalize(normal) + 1.0) / 2.0;
    fragColor.a = 1.0;
    return;
  } else {
    // Lookup normal and map from color components [0..1] to vector components
    // [-1..1].
    normal = normalize(texture(normalMap, fs_in.texCoords).xyz) * 2.0 - 1.0;
    normal = normalize(fs_in.fragTBN * normal);
    // TODO: Temporarily rendering the normal for debugging.
    fragColor.rgb = (normalize(normal) + 1.0) / 2.0;
    fragColor.a = 1.0;
    return;
  }

  // Shade with normal lights.
  vec3 result =
      qrk_shadeAllLights(material, fs_in.fragPos, normal, fs_in.texCoords);

  // Add emissions.
  result += qrk_shadeEmission(material, fs_in.fragPos, fs_in.texCoords);

  fragColor = vec4(result, qrk_materialAlpha(material, fs_in.texCoords));
  fragColor.rgb = qrk_gammaCorrect(fragColor.rgb);
}
