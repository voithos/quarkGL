#pragma once

#pragma qrk_include < lighting.frag>
#pragma qrk_include < pbr.frag>

#ifndef QRK_MAX_DIRECTIONAL_LIGHTS
#define QRK_MAX_DIRECTIONAL_LIGHTS 10
#endif
uniform QrkDirectionalLight qrk_directionalLights[QRK_MAX_DIRECTIONAL_LIGHTS];
uniform int qrk_directionalLightCount;

#ifndef QRK_MAX_POINT_LIGHTS
#define QRK_MAX_POINT_LIGHTS 10
#endif
uniform QrkPointLight qrk_pointLights[QRK_MAX_POINT_LIGHTS];
uniform int qrk_pointLightCount;

#ifndef QRK_MAX_SPOT_LIGHTS
#define QRK_MAX_SPOT_LIGHTS 10
#endif
uniform QrkSpotLight qrk_spotLights[QRK_MAX_SPOT_LIGHTS];
uniform int qrk_spotLightCount;