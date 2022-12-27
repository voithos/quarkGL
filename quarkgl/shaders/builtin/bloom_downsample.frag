#version 460 core

// Performs downsampling on a texture. This approach is taken from the technique
// used in Call of Duty presented at Siggraph 2014. This was used instead of
// simpler techniques (bilinear or box filtering) to avoid "pulsating artifacts
// and temporal stability issues."

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D qrk_bloomMipChain;

void main() {
  // Determine texel size.
  // When rendering from one mip level to another mip of the same texture,
  // calling code should use GL_TEXTURE_BASE_LEVEL and GL_TEXTURE_MAX_LEVEL to
  // limit the textures that can be sampled from. When doing this,
  // textureSize(..., 0) will correctly use the BASE mip level.
  vec2 srcTexelSize = 1.0 / vec2(textureSize(qrk_bloomMipChain, 0));
  float offsetX = srcTexelSize.x;
  float offsetY = srcTexelSize.y;

  // Take 13 samples around the current texel (ccc):
  //
  // etl --- ett --- etr
  // --- itl --- itr ---
  // ell --- ccc --- err
  // --- ibl --- ibr ---
  // ebl --- ebb --- ebr
  //
  // Convention:
  // - ccc: current
  // - itl: interior top left (etc)
  // - ebr: exterior bottom right (etc)

  // clang-format off
  vec4 etl = texture(qrk_bloomMipChain, vec2(texCoords.x - 2*offsetX, texCoords.y + 2*offsetY));
  vec4 ett = texture(qrk_bloomMipChain, vec2(texCoords.x,             texCoords.y + 2*offsetY));
  vec4 etr = texture(qrk_bloomMipChain, vec2(texCoords.x + 2*offsetX, texCoords.y + 2*offsetY));

  vec4 ell = texture(qrk_bloomMipChain, vec2(texCoords.x - 2*offsetX, texCoords.y            ));
  vec4 ccc = texture(qrk_bloomMipChain, vec2(texCoords.x,             texCoords.y            ));
  vec4 err = texture(qrk_bloomMipChain, vec2(texCoords.x + 2*offsetX, texCoords.y            ));

  vec4 ebl = texture(qrk_bloomMipChain, vec2(texCoords.x - 2*offsetX, texCoords.y - 2*offsetY));
  vec4 ebb = texture(qrk_bloomMipChain, vec2(texCoords.x,             texCoords.y - 2*offsetY));
  vec4 ebr = texture(qrk_bloomMipChain, vec2(texCoords.x + 2*offsetX, texCoords.y - 2*offsetY));

  vec4 itl = texture(qrk_bloomMipChain, vec2(texCoords.x - offsetX, texCoords.y + offsetY));
  vec4 itr = texture(qrk_bloomMipChain, vec2(texCoords.x + offsetX, texCoords.y + offsetY));
  vec4 ibl = texture(qrk_bloomMipChain, vec2(texCoords.x - offsetX, texCoords.y - offsetY));
  vec4 ibr = texture(qrk_bloomMipChain, vec2(texCoords.x + offsetX, texCoords.y - offsetY));
  // clang-format on

  // We want to weigh the sample amongst 5 square regions:
  // - 50% weight for 1 center region comprised of itl,itr,ibl,ibr (including
  //   the current texel).
  // - 12.5% weight each for 4 regions around the corners (for example, the
  //   top-left region of etl,ett,ell,ccc)
  //
  // However, if we just add up the samples naively, we'll double-count since
  // the regions overlap. To preserve energy, since each region is comprised of
  // exactly 5 samples, we redistribute the weights between all samples such
  // that:
  //   sample_weight =
  //       sum((region_weight / 5) for each region that sample is in)
  //
  // This yields the following distribution. The weights all add up to 1.
  fragColor = ccc * 0.2;
  fragColor += (itl + itr + ibl + ibr) * 0.125;
  fragColor += (ett + ell + err + ebb) * 0.05;
  fragColor += (etl + etr + ebl + ebr) * 0.025;
}
