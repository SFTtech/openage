# Openage graphics
The graphics subsystem is implemented in two levels. The first level is an abstraction over graphics APIs (OpenGL, Vulkan) and provides generic shader execution methods. The second level uses the first to draw openage-specific graphics, i.e. the actual world, units, etc.

![Renderer Hierarchy](images/renderer_hierarchy.svg)

## Level 1 Renderer

[More about the level 1 renderer](level1.md)

First things first, we might want to support multiple APIs. For now just OpenGL, but maybe Vulkan or some others.  We want to abstract over these, but this can't unfortunately be done at the level of graphics primitives like textures, buffers, etc. Well, it can, but it introduces unnecessary complexity and possible overhead. That is because the next-gen (Vulkan, Metal, DX12) APIs are vastly different from the old ones - most importantly, they're bindless, so something like a Vulkan context (GL notion) doesn't even make sense. We therefore choose to abstract on the higher level of things-to-draw.

It works similarly to the Unity engine. The user can submit resources to be uploaded to the GPU and receives a handle that identifies the uploaded resource. Resources can be added, updated and removed. Currently supported resource types: shader, texture.

## Level 2 Renderer

[More about the level 2 renderer](level2.md)

On top of the level 1 renderer, we build a level 2 graphics subsystem. It has an API that is specific to openage, and is threadsafe. The level 2 renderer calls the level 1 renderer and updates it to match the gamestate. It is part of the `Presenter` component of the engine.

Level 2 rendering involves several stages that each handle a slightly different use case. For example, the `TerrainRenderer` stage is used for drawing the terrain, the `WorldRenderer` is used for drawing sprites of units/buildings, etc. Each stage usually utilizes specific shaders and draws into its own framebuffer. The basic workflow and interface is roughly the same for all stages, with only slight differences in how they manage their internal render state.


### Namespaces:
#### Level 1 renderer
- `openage::renderer::opengl` - the OpenGL backend
- `openage::renderer::vulkan` - the Vulkan backend
- `openage::renderer::resources` - management of graphics assets
- `openage::renderer::resources::parser` - parsers for openage media [metadata files](/doc/media/openage)

##### Level 2 renderer
- `openage::renderer::skybox` - the background rendering stage
- `openage::renderer::terrain` - the terrain rendering stage
- `openage::renderer::world` - the unit/building rendering stage
- `openage::renderer::gui` - the GUI rendering stage
- `openage::renderer::screen` - final compositing of all level 2 rendering results for display on screen

Every namespace is an actual directory and all its classes are contained there.
