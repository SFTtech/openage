# Openage graphics
The graphics subsystem is implemented in two levels. The first level is an abstraction over graphics APIs (OpenGL, Vulkan) and provides generic shader execution methods. The second level uses the first to draw openage-specific graphics, i.e. the actual world, units, etc.

### Namespaces:
`openage::renderer` - the level 1 renderer
`openage::renderer::opengl` - the OpenGL implementation
`openage::renderer::vulkan` - the Vulkan implementation
`openage::renderer::resources` - management of graphics assets

__TODO name__
`openage::graphics` - the level 2 system

Every namespace is an actual directory and all its classes are contained there.

## Level 1:
### Overview
First things first, we might want to support multiple APIs. For now just OpenGL, but maybe Vulkan or some others.  We want to abstract over these, but this can't unfortunately be done at the level of graphics primitives like textures, buffers, etc. Well, it can, but it introduces unnecessary complexity and possible overhead. That is because the next-gen (Vulkan, Metal, DX12) APIs are vastly different from the old ones - most importantly, they're bindless, so something like a Vulkan context (GL notion) doesn't even make sense. We therefore choose to abstract on the higher level of things-to-draw.

It works similarly to the Unity engine. The user can submit resources to be uploaded to the GPU and receives a handle that identifies the uploaded resource. Resources can be added, updated and removed. Currently supported resource types: shader, texture.

### Thread-safety
This level might or might not be threadsafe depending on the concrete implementation. The OpenGL version is, in typical GL fashion, so not-threadsafe it's almost anti-threadsafe. All code must be executed sequentially on a dedicated window thread, the same one on which the window and renderer were initially created. The plan for the Vulkan version is to make it at least independent of thread-local storage and hopefully completely threadsafe.

### Usage
#### Renderer
All interaction with the renderer is done through the abstract `Renderer` class, initialized with a concrete implementation. For an OpenGL implementation, first create a window and then make a renderer for it:
```c++
opengl::GlWindow window("title", 1024, 768);
std::unique_ptr<Renderer> renderer = window.make_renderer();
```

The `opengl` namespace or any other implementation-specific namespace like `vulkan`` should not ever be used after initializing the window.

#### Resources
The `resources` namespace provides classes for initializing and loading meshes, textures, shaders, etc.
These objects can then be passed to the renderer to make them usable with graphics hardware, e.g.:
```c++
auto vshader_src = resources::ShaderSource(
    resources::shader_lang_t::glsl,
    resources::shader_stage_t::vertex,
    "#version 330\nvoid main() {}"
);

auto fshader_src = resources::ShaderSource(
    resources::shader_lang_t::glsl,
    resources::shader_stage_t::fragment,
    "#version 330\nvoid main() {}"
);

auto shader = renderer->add_shader( { vshader_src, fshader_src } );

auto tex = resources::Texture2dData(game_path / "/assets/gaben.png");
auto gpu_tex = renderer->add_texture(tex);
```

#### RenderPass and Renderable
Graphics operations are executed through submitting `RenderPass`es and `Renderable`s to the `Renderer` object. For details,
see `renderer.h`.

#### Sample usage:
Sample usage:

```c++
opengl::GlWindow window("title", 1024, 768');
std::unique_ptr<Renderer> renderer = window.make_renderer();

resources::TextureData tex_data(game_path / "/path.tex");
std::unique_ptr<Texture2d> tex = renderer->add_texture(tex_data);

resources::ShaderSource vsrc = resources::ShaderSource(
    resources::shader_lang_t::glsl,
    resources::shader_stage_t::vertex,
    game_path / "path.vert"
);
resources::ShaderSource fsrc = resources::ShaderSource(resources::shader_t::glsl_fragment);
    resources::shader_lang_t::glsl,
    resources::shader_stage_t::fragment,
    game_path / "path.frag"
);

std::unique_ptr<ShaderProgram> prog = renderer->add_shader( { vsrc, fsrc } );

std::unique_ptr<UniformInput> input = prog->new_uniform_input(
  "color", { 0.0f, 1.0f, 0.0f },
  "time", 0.0f,
  "num", 1337
);

std::unique_ptr<Geometry> geom = renderer->add_bufferless_quad();

RenderPass pass {
  { {
    input.get(),
    geom.get(),
    true,
    true,
  } },
  renderer->get_framebuffer_target(),
};

renderer->render(pass);
```

## Level 2:
On top of the level 1 renderer, we build a level 2 graphics subsystem. It has an API that is actually specific to Openage, and is threadsafe. The level-2 renderer calls the level 1 renderer and updates it to match the game state. In some documentation this is also called the "presenter".

## TODO:
- [x] Abstraction of render backend
  - [x] OpenGL 3.3 or higher
    - [ ] Use 4.x when available (optional)
    - [ ] Support ancient 2.x legacy (optional)
  - [ ] Vulkan (optional, #242)
- [ ] Render pipeline abstraction
  - [x] Textures
  - [x] Shaders
    - [x] Uniforms
    - [ ] Uniform buffers
    - [x] Vertex attributes
  - [x] Geometries
    - [x] Quad primitives
    - [ ] Circles
    - [ ] Smooth paths
    - [ ] Mesh importing (optional)
  - [x] Render targets
    - [x] Framebuffers
    - [ ] Renderbuffers (optional)
- [ ] Functionality
  - [x] Screenshot support
    - [ ] [PBO optimization](http://www.songho.ca/opengl/gl_pbo.html) for texture downloading (optional)
  - [x] Pixel-perfect unit hitbox for unit selection and damage areas (#368, #671 )
  - [ ] Outline rendering
  - [ ] Investigate why tree textures render incorrectly (#359, [maybe this?](http://www.adriancourreges.com/blog/2017/05/09/beware-of-transparent-pixels/))
  - [ ] Fix #374
- [ ] Terrain rendering
  - [ ] Merge terrain texture into a single bitmap
  - [ ] Cache blending results (#154, #158)
  - [ ] Do as much as possible in shaders (#149)
  - [ ] Clip tiles properly (#141)
- [ ] Optimizations
  - [ ] [Occlusion queries](https://vertostudio.com/gamedev/?p=177)
  - [ ] Minimize OpenGL state changes (batch by shader, then by buffer)
  - [ ] Texture binpacking into atlas
  - [ ] Smooth zooming
- [ ] Integration
  - [ ] Rewrite all of drawing functionality to be expressed in terms of `Renderer`
    - [ ] Get rid of GL code everywhere except the rendering backend and the GUI
    - [ ] Write a `GameRenderer` that takes evaluations of curves at the current instant as input
  - [ ] GUI integration
    - [ ] TBD (#624)
