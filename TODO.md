# TODOs

- [ ] Add a 'light' class that can work with the shader.
- [ ] Add unit tests.
- [ ] Add more documentation.
- [ ] Add a 'window' class to qrk that can encapsulate the calls in the render loop.
- [ ] Add a mechanism for working with textures.
- [ ] Add a material class that can maintain diffuse/specular maps.

## Done
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
