# Openage graphics
The graphics subsystem is implemented in two levels. The first level is an abstraction over graphics APIs (OpenGL, Vulkan) and provides generic shader execution methods. The second level uses the first to draw openage-specific graphics, i.e. the actual world, units, etc.

### Namespaces:
#### Level 1 renderer
- `openage::renderer::opengl` - the OpenGL implementation
- `openage::renderer::vulkan` - the Vulkan implementation
- `openage::renderer::resources` - management of graphics assets

##### Level 2 renderer
- `openage::renderer::skybox` - the background rendering stage
- `openage::renderer::terrain` - the terrain rendering stage
- `openage::renderer::world` - the unit/building rendering stage
- `openage::renderer::gui` - the GUI rendering stage
- `openage::renderer::screen` - final compositing of all level 2 rendering results for display on screen

Every namespace is an actual directory and all its classes are contained there.

## Level 1
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

Renderable obj {
  input.get(),
  geom.get(),
  true,
  true,
}

std::unique_ptr<RenderPass> pass =  renderer->add_render_pass({ obj }, renderer->get_framebuffer_target())

renderer->render(pass);
```

## Level 2
On top of the level 1 renderer, we build a level 2 graphics subsystem. It has an API that is specific to openage, and is threadsafe. The level 2 renderer calls the level 1 renderer and updates it to match the gamestate. It is part of the `Presenter` component of the engine.

Level 2 rendering involves several stages that each handle a slightly different use case. For example, the `TerrainRenderer` stage is used for drawing the terrain, the `WorldRenderer` is used for drawing sprites of units/buildings, etc. Each stage usually utilizes specific shaders and draws into its own framebuffer. The basic workflow and interface is roughly the same for all stages, with slight differences in how they manage their internal render state.

### Stages

Every stage has its own subrenderer that manages a `RenderPass` from the level 1 renderer and updates it with `Renderable`s created using update information from the gamestate. Stages also store the vertex and fragment shaders used for drawing the renderable objects.

There are currently 5 stages in the level 2 rendering pipeline:

1. `SkyboxRenderer`: Draws the background behind the terrain (as a single color).
1. `TerrainRenderer`: Draws the terrain. Terrains are handled as textured 3D meshes.
1. `WorldRenderer`: Draws animations and sprites for units/buildings and other 2D ingame objects.
1. `GuiRenderer`: Draws the GUI overlay. The drawing part in this stage is actually done by Qt, while the level 1 renderer only provides the framebuffer.
1. `ScreenRenderer`: Alpha composites the framebuffer data of previous stages and draws them onto the screen (i.e. it overlays the outputs from the other stages).

With the exception of the `ScreenRenderer`, all stages an work independently from each other.


### Communication with the Gamestate