# TODOs

## Features
- [ ] P0: Model render features
  - [ ] Rotate/scale
  - [x] Camera mode
  - [x] Ambient color controls
  - [x] Directional light color / position / etc controls
  - [ ] Various algorithm params?
  - [ ] Replace model via file upload
  - [x] Switch to deferred?
  - [x] Display deferred render buffers
  - [x] Add PBR
  - [ ] Add bloom
  - [ ] Emission strength
  - [ ] Tone mapping options
  - [ ] Gamma correct
  - [ ] Add shadow mapping
  - [ ] Add SSAO
- [ ] P0: Update README / documentation
- [ ] P0: Implement IBL (image based lighting)
- [ ] P0: Add FXAA
- [ ] P1: Add skeletal animation https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
- [ ] P1: Implement tessellation
- [ ] P1: Implement material system
- [ ] P1: Add parallax mapping
- [ ] P1: Implement point light shadow maps: https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
- [ ] P1: Add profiler (easy_profiler?)
- [ ] P1: Add screen space reflections
- [ ] P2: Implement Light volumes
- [ ] P2: Implement CSM: https://learnopengl.com/Guest-Articles/2021/CSM
- [ ] P2: Don't store positions in the G-Buffer: https://mynameismjp.wordpress.com/2010/09/05/position-from-depth-3/
- [ ] P2: Implement virtual textures. http://holger.dammertz.org/stuff/notes_VirtualTexturing.html
- [ ] P2: Add a scene graph. https://learnopengl.com/Guest-Articles/2021/Scene/Scene-Graph
- [ ] P2: Expose scene graph in model_render UI
- [ ] P2: Implement frustum culling. https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
- [ ] P2: Consider supporting uniform buffer objects. https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
- [ ] P2: Go through some of the shader effects from https://lettier.github.io/3d-game-shaders-for-beginners/index.html
- [ ] P2: Add automatic exposure: https://bruop.github.io/exposure/
- [ ] P3: Add env map lighting support. https://learnopengl.com/Advanced-OpenGL/Cubemaps
- [ ] P3: Area lights https://learnopengl.com/Guest-Articles/2022/Area-Lights
- [ ] P3: Tons of shadow map improvements described here: https://docs.microsoft.com/en-us/windows/win32/dxtecharts/common-techniques-to-improve-shadow-depth-maps
- [ ] P4: Add weighted, blended order-independent transparency (http://casual-effects.blogspot.com/2015/03/implemented-weighted-blended-order.html)

## Cleanup
- [ ] P1: Add `#pragma once` for shaders
- [ ] P1: Model loading is sometimes slow
- [ ] P2: Add a logging system, and log e.g. invalid glGetUniformLocation() calls
- [ ] P2: Remove some duplication from light class impl
- [ ] P3: Add a mechanism to set #define's in the shader

## Examples
- [ ] Lighting demo (light types, spotlight)
- [ ] Stencil buffer demo
- [ ] Blending (alpha transparency) demo

## Bugs
- [ ] P2: light registry doesn't work with >1 shaders.
- [ ] P2: debug normals are modified based on camera rotation. Maybe just a visual artifact?

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
- [x] P2: Add a texturemap class that can maintain diffuse/specular maps, similar to the light class.
- [x] P1: Add Bloom
- [x] P0: model loader doesn't honor transforms
- [x] P3: Add instancing to model?
- [x] P0: Implement deferred shading
- [x] P2: Add dynamic UI support, a la https://github.com/ocornut/imgui
- [x] P0: Orbit controls
- [x] P0: Implement SSAO
- [x] P0: Physically based bloom: https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom
- [x] P0: Implement PBR lighting
- [x] P0: Create PBR example
- [x] P0: Implement PBR texture maps
- [x] P0: Add support for AO maps
- [x] P0: Banding in specular when using GL_RGBA16F with deferred shading
- [x] P3: Add a wireframe mode.
- [x] P0: Emissive textures have artifacts at close range
