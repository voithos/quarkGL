# TODOs

- [ ] Allow rendering a VAO with a given shader.
- [ ] Add a wireframe mode.
- [ ] Add a mechanism for working with textures.
- [ ] Add support for framebuffers.
- [ ] Add a mechanism to set #define's in the shader
- [ ] Use GLuint, GLenum, GLint, etc
- [ ] Add a logging system, and log e.g. invalid glGetUniformLocation() calls
- [ ] Remove some duplication from light class impl
- [ ] Add a material class that can maintain diffuse/specular maps, similar to the light class.
- [ ] Add a core function (on window?) that calls a callback with a try/catch around for exception output
- [ ] Getting duplicate items in compile_commands.json.
- [ ] Add unit tests.
- [ ] Add more documentation.
- [ ] Add logging support.
- [ ] Add weighted, blended order-independent transparency (http://casual-effects.blogspot.com/2015/03/implemented-weighted-blended-order.html)
- [ ] Add stacktraces to exceptions.
- [ ] Consider supporting uniform buffer objects.
- [ ] Add UI for adjusting params (akin to dat gui?).
- [ ] Bug: debug normals are modified based on camera rotation.

## Examples to make

- Lighting demo (light types, spotlight)
- Stencil buffer demo
- Blending (alpha transparency) demo

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
