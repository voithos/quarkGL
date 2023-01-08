#version 460 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D qrk_screenTexture;

float lumaFromGammaCompressed(vec3 color) {
  // sqrt is an approximate inverse gamma transformation.
  return sqrt(dot(color, vec3(0.299, 0.587, 0.114)));
}

// Taken from
// http://blog.simonrodriguez.fr/articles/2016/07/implementing_fxaa.html.
const float EDGE_THRESHOLD_MIN = 0.0312;
const float EDGE_THRESHOLD_MAX = 0.125;
const float SUBPIXEL_QUALITY = 0.75;

const int MAX_ITERATIONS = 12;
const float STEP_SIZE[12] =
    float[](1.0, 1.0, 1.0, 1.0, 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0);

void main() {
  vec2 texelSize = 1.0 / vec2(textureSize(qrk_screenTexture, 0));

  vec4 colorCenter = texture(qrk_screenTexture, texCoords);

  float lumaCenter = lumaFromGammaCompressed(colorCenter.rgb);

  // Luma at four direct cardinal neighbors.
  // clang-format off
  float lumaDown =  lumaFromGammaCompressed(textureOffset(qrk_screenTexture, texCoords, ivec2( 0, -1)).rgb);
  float lumaUp =    lumaFromGammaCompressed(textureOffset(qrk_screenTexture, texCoords, ivec2( 0,  1)).rgb);
  float lumaLeft =  lumaFromGammaCompressed(textureOffset(qrk_screenTexture, texCoords, ivec2(-1,  0)).rgb);
  float lumaRight = lumaFromGammaCompressed(textureOffset(qrk_screenTexture, texCoords, ivec2( 1,  0)).rgb);
  // clang-format on

  // Find the min and max luma around the current fragment.
  float lumaMin =
      min(lumaCenter, min(min(lumaDown, lumaUp), min(lumaLeft, lumaRight)));
  float lumaMax =
      max(lumaCenter, max(max(lumaDown, lumaUp), max(lumaLeft, lumaRight)));

  float lumaRange = lumaMax - lumaMin;

  // We base the delta check proportionally to the max luma, but skip really
  // dark areas (or else we'd be antialiasing even subtle changes).
  if (lumaRange < max(EDGE_THRESHOLD_MIN, lumaMax * EDGE_THRESHOLD_MAX)) {
    fragColor = colorCenter;
    return;
  }

  // Query the four corners.
  // clang-format off
  float lumaDownLeft =  lumaFromGammaCompressed(textureOffset(qrk_screenTexture, texCoords, ivec2(-1, -1)).rgb);
  float lumaUpRight =   lumaFromGammaCompressed(textureOffset(qrk_screenTexture, texCoords, ivec2( 1,  1)).rgb);
  float lumaUpLeft =    lumaFromGammaCompressed(textureOffset(qrk_screenTexture, texCoords, ivec2(-1,  1)).rgb);
  float lumaDownRight = lumaFromGammaCompressed(textureOffset(qrk_screenTexture, texCoords, ivec2( 1, -1)).rgb);
  // clang-format on

  // Next we find the gradient. Precompute some reused values.
  float lumaDownUp = lumaDown + lumaUp;
  float lumaLeftRight = lumaLeft + lumaRight;

  float lumaLeftCorners = lumaDownLeft + lumaUpLeft;
  float lumaRightCorners = lumaDownRight + lumaUpRight;
  float lumaUpCorners = lumaUpLeft + lumaUpRight;
  float lumaDownCorners = lumaDownLeft + lumaDownRight;

  // Compute a gradient estimate for horizontal or vertical edges.
  // - A horizontal edge separates "horizontal" groups of pixels - thus, we want
  //   to compare pixels vertically.
  // - A vertical edge separates "vertical" groups of pixels - thus, we want to
  //   compare pixels horizontally.
  // clang-format off
  float horizontalDelta = abs(lumaLeftCorners - 2.0 * lumaLeft) + abs(lumaDownUp    - 2.0 * lumaCenter) * 2.0 + abs(lumaRightCorners - 2.0 * lumaRight);
  float verticalDelta   = abs(lumaUpCorners   - 2.0 * lumaUp)   + abs(lumaLeftRight - 2.0 * lumaCenter) * 2.0 + abs(lumaDownCorners  - 2.0 * lumaDown);
  // clang-format on

  bool isHorizontalEdge = horizontalDelta >= verticalDelta;

  // Luma 1 is in the negative direction, 2 in the positive.
  float luma1, luma2;
  if (isHorizontalEdge) {
    luma1 = lumaDown;
    luma2 = lumaUp;
  } else {
    luma1 = lumaLeft;
    luma2 = lumaRight;
  }

  float gradient1 = abs(luma1 - lumaCenter);
  float gradient2 = abs(luma2 - lumaCenter);

  bool is1Steepest = gradient1 >= gradient2;

  float gradientScaled = 0.25 * max(gradient1, gradient2);
  float texOffset = isHorizontalEdge ? texelSize.y : texelSize.x;

  // Average luma in the steepest direction.
  float lumaLocalAverage = 0.0;
  if (is1Steepest) {
    texOffset *= -1.0;
    lumaLocalAverage = 0.5 * (luma1 + lumaCenter);
  } else {
    lumaLocalAverage = 0.5 * (luma2 + lumaCenter);
  }

  // Shift by half a texel towards the steepest gradient.
  vec2 shiftedCoords = texCoords;
  if (isHorizontalEdge) {
    shiftedCoords.y += texOffset * 0.5;
  } else {
    shiftedCoords.x += texOffset * 0.5;
  }

  // Compute shifted UVs to explore along the sides of the edge, orthogonally.
  vec2 edgeOffset =
      isHorizontalEdge ? vec2(texelSize.x, 0.0) : vec2(0.0, texelSize.y);
  vec2 edgeCoords1 = shiftedCoords - edgeOffset;
  vec2 edgeCoords2 = shiftedCoords + edgeOffset;

  float lumaEnd1;
  float lumaEnd2;
  bool reached1 = false;
  bool reached2 = false;
  for (int i = 0; i < MAX_ITERATIONS; ++i) {
    if (!reached1) {
      lumaEnd1 =
          lumaFromGammaCompressed(texture(qrk_screenTexture, edgeCoords1).rgb);
      lumaEnd1 -= lumaLocalAverage;
    }
    if (!reached2) {
      lumaEnd2 =
          lumaFromGammaCompressed(texture(qrk_screenTexture, edgeCoords2).rgb);
      lumaEnd2 -= lumaLocalAverage;
    }

    // Check if we have reached the end of the edge.
    reached1 = abs(lumaEnd1) >= gradientScaled;
    reached2 = abs(lumaEnd2) >= gradientScaled;

    // If we haven't reached the end of the edge, shift the UVs by an increasing
    // number of texels.
    if (!reached1) {
      edgeCoords1 -= edgeOffset * STEP_SIZE[i];
    }
    if (!reached2) {
      edgeCoords2 += edgeOffset * STEP_SIZE[i];
    }
    if (reached1 && reached2) {
      break;
    }
  }

  // Compute the UV distances to the edge extremities.
  float distance1 = isHorizontalEdge ? texCoords.x - edgeCoords1.x
                                     : texCoords.y - edgeCoords1.y;
  float distance2 = isHorizontalEdge ? edgeCoords2.x - texCoords.x
                                     : edgeCoords2.y - texCoords.y;

  bool is1Closer = distance1 < distance2;
  float distanceClosest = min(distance1, distance2);

  float edgeLength = distance1 + distance2;
  float coordOffset = -distanceClosest / edgeLength + 0.5;

  bool isLumaCenterSmaller = lumaCenter < lumaLocalAverage;
  bool correctVariation =
      ((is1Closer ? lumaEnd1 : lumaEnd2) < 0.0) != isLumaCenterSmaller;

  // Don't offset if the variation is incorrect.
  float finalOffset = correctVariation ? coordOffset : 0.0;

  // Sub-pixel antialiasing.
  float lumaFullAverage = (1.0 / 12.0) * (2.0 * (lumaDownUp + lumaLeftRight) +
                                          lumaLeftCorners + lumaRightCorners);
  float subpixelOffset1 =
      clamp(abs(lumaFullAverage - lumaCenter) / lumaRange, 0.0, 1.0);
  float subpixelOffset2 =
      (-2.0 * subpixelOffset1 + 3.0) * subpixelOffset1 * subpixelOffset1;
  float subpixelOffsetFinal =
      subpixelOffset2 * subpixelOffset2 * SUBPIXEL_QUALITY;

  finalOffset = max(finalOffset, subpixelOffsetFinal);

  vec2 finalCoords = texCoords;
  if (isHorizontalEdge) {
    finalCoords.y += finalOffset * texOffset;
  } else {
    finalCoords.x += finalOffset * texOffset;
  }

  fragColor = texture(qrk_screenTexture, finalCoords);
}
