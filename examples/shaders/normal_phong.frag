#version 460 core
#pragma qrk_include < core.glsl>
#pragma qrk_include < normals.frag>
#pragma qrk_include < standard_lights.frag>
#pragma qrk_include < depth.frag>

// An example fragment shader that uses normal mapping.

in VS_OUT {
  vec2 texCoords;
  vec3 fragPos_viewSpace;
  vec3 fragNormal_viewSpace;
  mat3 fragTBN_viewSpace;  // Transforms from tangent frame to view frame.
}
fs_in;

out vec4 fragColor;

uniform QrkMaterial material;
uniform sampler2D normalMap;
uniform bool useVertexNormals;
uniform bool renderNormals;

void main() {
  // Calculate the normal, either using vertex normals or the normal map.
  vec3 normal_viewSpace;
  if (useVertexNormals) {
    normal_viewSpace = normalize(fs_in.fragNormal_viewSpace);
  } else {
    // Lookup normal and map from tangent space to view space.
    normal_viewSpace =
        normalize(fs_in.fragTBN_viewSpace *
                  qrk_sampleNormalMap(normalMap, fs_in.texCoords));
  }

  // Optionally render normals instead of lighting.
  if (renderNormals) {
    fragColor = qrk_normalColor(normal_viewSpace);
    return;
  }

  // Shade with normal lights.
  vec3 result = qrk_shadeAllLights(material, fs_in.fragPos_viewSpace,
                                   normal_viewSpace, fs_in.texCoords);

  // Add emissions.
  result +=
      qrk_shadeEmission(material, fs_in.fragPos_viewSpace, fs_in.texCoords);

  fragColor = vec4(result, qrk_materialAlpha(material, fs_in.texCoords));
  fragColor.rgb = qrk_gammaCorrect(fragColor.rgb);
}
