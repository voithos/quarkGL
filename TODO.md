# TODOs

## Features
- [ ] P0: Implement deferred shading
- [ ] P0: Implement SSAO
- [ ] P0: Implement PBR lighting
- [ ] P0: Implement tessellation
- [ ] P1: Implement point light shadow maps: https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
- [ ] P1: Add parallax mapping
- [ ] P1: Add env map lighting support. https://learnopengl.com/Advanced-OpenGL/Cubemaps
- [ ] P2: Implement CSM
- [ ] P2: Implement virtual textures. http://holger.dammertz.org/stuff/notes_VirtualTexturing.html
- [ ] P2: Add skeletal animation https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
- [ ] P2: Add a scene graph. https://learnopengl.com/Guest-Articles/2021/Scene/Scene-Graph
- [ ] P2: Implement frustum culling. https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
- [ ] P2: Consider supporting uniform buffer objects. https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
- [ ] P2: Go through some of the shader effects from https://lettier.github.io/3d-game-shaders-for-beginners/index.html
- [ ] P2: Add dynamic UI support, a la https://github.com/ocornut/imgui
- [ ] P2: Add automatic exposure: https://bruop.github.io/exposure/
- [ ] P3: Implement IBL (image based lighting)
- [ ] P3: Tons of shadow map improvements described here: https://docs.microsoft.com/en-us/windows/win32/dxtecharts/common-techniques-to-improve-shadow-depth-maps
- [ ] P3: Improve blur used for bloom
- [ ] P3: Update README / documentation
- [ ] P3: Add a wireframe mode.
- [ ] P3: Add instancing to model?
- [ ] P4: Add weighted, blended order-independent transparency (http://casual-effects.blogspot.com/2015/03/implemented-weighted-blended-order.html)

## Cleanup
- [ ] P2: Add a logging system, and log e.g. invalid glGetUniformLocation() calls
- [ ] P2: Remove some duplication from light class impl
- [ ] P3: Add a mechanism to set #define's in the shader

## Examples
- [ ] Lighting demo (light types, spotlight)
- [ ] Stencil buffer demo
- [ ] Blending (alpha transparency) demo

## Bugs
- [ ] P2: light registry doesn't work with >1 shaders.
- [ ] P2: debug normals are modified based on camera rotation.

## Done
- [x] Bug: debug normal disappear in weird ways.
- [x] Add a way to load shader code from a string (with import support).
- [x] Add deltaTime uniforms, etc
- [x] Add support for the stencil buffer
- [x] Add a mechanism to enable/disable depth testing
- [x] Add a 'window' class to qrk that can encapsulate the calls in the render loop.
- [x] Add some depth-related shader functions
- [x] Shading doesn't look quite right
- [x] Fix model textures.
- [x] getViewMatrix -> getViewTransform
- [x] Figure out what's going on with double-free.
- [x] Add a 'light' class that can work with the shader.
- [x] Add caching for shader loader.
- [x] Add an abstraction over shader code, allowing e.g. imports / usage.
- [x] Add exception hierarchy.
- [x] Add an auto-formatter for the GLSL code.
- [x] Include attenuation in the lighting model.
- [x] Add lighting support to qrk, allowing:
  - [x] Directional lights
  - [x] Point lights
  - [x] Spotlight lights
- [x] Switch to using .vert and .frag for shader extensions.
- [x] use/unuse -> activate/deactive.
- [x] Add clang-tidy.
- [x] Reprioritize TODOs :o)
- [x] P0: Add a mechanism for working with textures.
- [x] Add skybox support.
- [x] Add compute shaders.
- [x] P0: Implement blinn phong
- [x] P0: Add shadow mapping
- [x] P0: Add normal mapping
- [x] P1: Add HDR
- [x] P3: Add support for framebuffers?
- [x] P2: Add a material class that can maintain diffuse/specular maps, similar to the light class.
- [x] P1: Add Bloom
