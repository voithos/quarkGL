# TODOs

- [ ] Fix model textures.
- [ ] Use GLuint, GLenum, GLint, etc
- [ ] Add a logging system, and log e.g. invalid glGetUniformLocation() calls
- [ ] Remove some duplication from light class impl
- [ ] Add a material class that can maintain diffuse/specular maps, similar to the light class.
- [ ] Add deltaTime uniforms, etc
- [ ] Add a core function (on window?) that calls a callback with a try/catch around for exception output
- [ ] Getting duplicate items in compile_commands.json.
- [ ] Add unit tests.
- [ ] Add more documentation.
- [ ] Add a 'window' class to qrk that can encapsulate the calls in the render loop.
- [ ] Add a mechanism for working with textures.
- [ ] Add logging support.

## Done
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
