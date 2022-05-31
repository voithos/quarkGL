# TODOs

## Features
- [ ] P0: Add a mechanism for working with textures.
- [ ] P1: Consider supporting uniform buffer objects.
- [ ] P1: Allow rendering a VAO with a given shader.
- [ ] P1: Add a wireframe mode.
- [ ] P1: Add support for framebuffers.
- [ ] P1: Add a material class that can maintain diffuse/specular maps, similar to the light class.
- [ ] P2: Go through some of the shader effects from https://lettier.github.io/3d-game-shaders-for-beginners/index.html
- [ ] P2: Add dynamic UI support, a la https://github.com/ocornut/imgui
- [ ] P3: Update README / documentation
- [ ] P3: Add instancing to model.
- [ ] P4: Add weighted, blended order-independent transparency (http://casual-effects.blogspot.com/2015/03/implemented-weighted-blended-order.html)

## Cleanup
- [ ] P2: Add a logging system, and log e.g. invalid glGetUniformLocation() calls
- [ ] P2: Remove some duplication from light class impl
- [ ] P2: Use GLuint, GLenum, GLint, etc
- [ ] P3: Add a mechanism to set #define's in the shader
- [ ] P3: Add stacktraces to exceptions.
- [ ] P4: Add a core function (on window?) that calls a callback with a try/catch around for exception output

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
