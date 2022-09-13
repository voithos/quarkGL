#version 460 core
#pragma qrk_include < core.glsl>
#pragma qrk_include < normals.frag>
#pragma qrk_include < standard_lights.frag>
#pragma qrk_include < depth.frag>

// An example fragment shader for rendering models.

in VS_OUT {
  vec2 texCoords;
  vec3 fragPos_viewSpace;
  vec3 fragNormal_viewSpace;
  mat3 fragTBN_viewSpace;  // Transforms from tangent frame to view frame.
}
fs_in;

out vec4 fragColor;

uniform QrkMaterial material;
uniform bool useVertexNormals;

void main() {
  vec3 normal_viewSpace;
  if (useVertexNormals) {
    normal_viewSpace = normalize(fs_in.fragNormal_viewSpace);
  } else {
    // Lookup normal and map from tangent space to view space.
    normal_viewSpace =
        qrk_getNormal(material, fs_in.texCoords, fs_in.fragTBN_viewSpace,
                      fs_in.fragNormal_viewSpace);
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
