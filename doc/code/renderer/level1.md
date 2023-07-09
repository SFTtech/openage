#  Level 1 Renderer

Low-level renderer for communicating with the OpenGL and Vulkan APIs.

## Overview

1. [Level 1 Renderer](#level-1-renderer)
   1. [Overview](#overview)
   2. [Architecture](#architecture)
   3. [Basic  Usage](#basic--usage)
      1. [Window/Renderer Creation](#windowrenderer-creation)
      2. [Adding a Shader Program](#adding-a-shader-program)
      3. [Creating a Renderable](#creating-a-renderable)
      4. [Rendering and Displaying the Result](#rendering-and-displaying-the-result)
   4. [Advanced Usage](#advanced-usage)
      1. [Framebuffers / Multiple Render Passes](#framebuffers--multiple-render-passes)
      2. [Complex Geometry](#complex-geometry)
      3. [Uniform Buffers](#uniform-buffers)
   5. [Thread-safety](#thread-safety)


## Architecture

The abstract interface can be vaguely categorized into 4 important components:

1. Abstract interface
1. OpenGL backend
1. Vulkan backend
1. Resources

The abstract interface of the level 1 renderer provides a unified interface to the low-level
rendering capabilities of the GPU. It is independent of the specific backends, i.e. usage is the
same no matter which backend is selected at runtime. The level 2 renderer should always use
the abstract interface and should rarely, if ever, have to interact with the OpenGL/Vulkan backends
themselves. At runtime, the interface instantiates either an OpenGL or a Vulkan backend depending
on what graphics API is available.

Both the OpenGL and Vulkan backends are, for the most part, direct implementations of the abstract interface
in their domain-specific APIs. We won't go into much detail about their features in this document,
especially since there are much better explanations of the concepts behind both [OpenGL](https://learnopengl.com/Introduction)
and [Vulkan](https://vulkan-tutorial.com/Introduction) available.

The `resources` namespace provides classes for initializing and loading meshes, textures, shaders, etc.
These classes are independent from the specific OpenGL/Vulkan backends and can thus be passed to the
abstract interface of the renderer renderer to make them usable with graphics hardware.

## Basic  Usage

- see demos in libopenage/renderer/demo

### Window/Renderer Creation

To create the renderer, we first have to create a window that the renderer can draw into. This should be done
with the static method `renderer::Window::create(..)` which also automatically determines whether the
OpenGL or Vulkan backend should be used.

```c++
std::shared_ptr<Window> window = Window::create("title", 1024, 768);
```

By default, OpenGL is chosen, so the created window object for the above example should have type
`renderer::opengl::GlWindow`. The specific type should never be relevant for usage, however, since
all interaction can be done via the abstract interface.

The created window can be used to add and initialize a renderer, returning a pointer to the
abstract `renderer::Renderer` interface.

```c++
std::shared_ptr<Renderer> renderer = window->make_renderer();
```

Behind the scene, this again is initialized as a specific type determined by the type of the
window implementation behind the scenes. I.e., a `GlWindow` will make a `GlRenderer` when
calling this method.


### Adding a Shader Program

Shaders can be added in two steps. First, we have to load the shader source code from a
string or a file. To do this, we can use the `renderer::resources::ShaderSource` class as
seen below.

```c++
resources::ShaderSource vshader_src = resources::ShaderSource(
    resources::shader_lang_t::glsl,
    resources::shader_stage_t::vertex,
    "#version 330\nvoid main() {}"
);

util::Path shader_path = root_dir / "assets" / "shaders";
resources::ShaderSource fshader_src = resources::ShaderSource(
    resources::shader_lang_t::glsl,
    resources::shader_stage_t::fragment,
    shader_path / "source.frag"
);
```

Afterwards, we can create the shader program from the shader sources by passing them to
the `add_shader()` method of our renderer instantiation.

```c++
std::shared_ptr<ShaderProgram> shader_prog = renderer->add_shader( { vshader_src, fshader_src } );
```

openage's `ShaderProgram` encapsulates all shader units that should be run in one iteration
of the OpenGL/Vulkan graphics pipeline. Therefore, we have to supply at least a vertex shader
and a fragment shader source, since these stages are mandatory in both the OpenGL and the
Vulkan graphics pipeline.


### Creating a Renderable

To use our shader program, we have to define something that it can operate on. These "somethings"
are called *renderables* in openage and basically represent an object that we want to draw on screen.
Creating a `renderer::Renderable` object only requires the definition of two parameters:

1. Vertex inputs for the vertex shader stage
1. Uniform inputs for any defined uniforms in the shader stages of the shader program

The renderer provides a method to create a simple 4-vertex quad that spans the entire
viewport. Doing this creates a `renderer::Geometry` that also manages the underlying buffers
on the GPU and the associated vertex data:

```c++
std::shared_ptr<Geometry> geom = renderer->add_bufferless_quad();
```

Bufferless quads are usually enough to draw anything rectangular, e.g. sprites. However,
the renderer is also able to handle more complex geometry with changing vertex data
which is described in [this section](#complex-geometry).

Uniform inputs are created from the shader program that they are defined in by calling
`renderer::ShaderProgram::new_uniform_input(..)` on it. This creates a new `UniformInput` object
that will store the unique uniform input values for the renderable that we want to display.
Usually, a new `UniformInput` object should be created for each renderable.

```c++
std::shared_ptr<UniformInput> input = shader_prog->new_uniform_input(
  "color", Eigen::Vector3f{ 0.0f, 1.0f, 0.0f },
  "time", 0.0f,
  "num", 1337
);
```

Input values are passed to the method in pairs consisting of the uniform variable
name (as a string) and the input value. The definition order doesn't matter and the
method doesn't differentiate between different shader stages, so uniform inputs
for vertex and fragment shaders can be freely mixed.

Uniform input values are automatically converted to the correct types expected by the uniform
definition, e.g. a `uint8_t` for a uniform with type `uint` will be transformed to the
correct type.

After creating a `renderer::UniformInput`, it can be updated at any time, e.g. when preparing
the next frame:

```c++
input->update(
  "condition", false
);
```

From the geometry and the uniform input objects, we can finally create the `renderer::Renderable`
object that we want to display.

```c++
Renderable obj {
  input,
  geom
};
```

### Rendering and Displaying the Result

Graphics operations using a shader program are executed by organizing renderables in a *render pass*.
Render passes render multiple objects into a single display target, e.g. the application window.

Creating a render pass from the renderer only requires passing a list of renderables and the
display target that should be used. The window the renderer was created from is the default
display target and can be acquired by calling `renderer::Renderer::get_display_target()`.

```c++
std::shared_ptr<RenderPass> pass =  renderer->add_render_pass({ obj }, renderer->get_display_target())
```

Render passes can also be updated with new renderables:

```c++
pass->add_renderables({ obj });
```

Finally, we can execute the rendering pipeline for all objects in the render pass:

```c++
renderer->render(pass);
```

Before rendering, the render pass optimizes the order in which renderables are rendered to
minimize state changes on the GPU and save computation time. For example, the render pass
sorts the renderables by shader program, since changing them is an expensive operation.

After rendering is finished, the window has to be updated to display the rendered result.

```c++
window->update();
```

## Advanced Usage

These are some of the more advanced features of the renderer.

### Framebuffers / Multiple Render Passes

Sometimes it is useful to render the scene in multiple passes, e.g. for post-processing
of rendered objects or simply for organizing different rendering stages. To do this,
a render pass can be instructed to render into an intermediary texture attached to a
framebuffer.

```c++
std::shared_ptr<Window> window = Window::create("title", 1024, 768);
std::shared_ptr<Renderer> renderer = window->make_renderer();

... // shader program initialization

std::shared_ptr<Geometry> geom = renderer->add_bufferless_quad();
std::shared_ptr<UniformInput> input1 = shader_prog->new_uniform_input();
Renderable obj1{input1, geom};

std::shared_ptr<Texture2d> color_texture = renderer->add_texture(
  resources::Texture2dInfo(
    1024,
    768,
    resources::pixel_format::rgba8
  )
);
std::shared_ptr<RenderTarget> target = renderer->create_texture_target({ color_texture });
std::shared_ptr<RenderPass> pass1 = renderer->add_render_pass({ obj1 }, target);
```

The color texture assigned as the display target for the render pass can be assigned
as a uniform input value in subsequent render passes.

```c++
std::shared_ptr<UniformInput> input2 = shader_prog->new_uniform_input(
  "tex", texture
);
Renderable obj2{input2, geom};
std::shared_ptr<RenderPass> pass2 = renderer->add_render_pass({ obj2 }, renderer->get_display_target());
```

A color texture is not the only type of texture that can be assigned to a texture
target. We can also add depth textures or additional (color) textures that the shader
can write arbritrary values into.

```c++
std::shared_ptr<Texture2d> depth_texture = renderer->add_texture(
  resources::Texture2dInfo(
    1024,
    768,
    resources::pixel_format::depth24 // 24 Bit depth values
  )
);
std::shared_ptr<Texture2d> id_texture = renderer->add_texture(
  resources::Texture2dInfo(
    1024,
    768,
    resources::pixel_format::r32ui // unsigned integer
  )
);
std::shared_ptr<RenderTarget> target = renderer->create_texture_target({color_texture, depth_texture, id_texture});
std::shared_ptr<RenderPass> pass = renderer->add_render_pass({ obj }, target);
```

Attaching a depth texture is required for enabling optional depth testing for a renderable.
Depth testing can be activated per renderable:

```c++
Renderable obj {
  input,
  geom
};
obj.depth_test = true;
```

### Complex Geometry

For displaying complex geometry like 3D objects or non-rectangular surfaces, the renderer
allows the definition meshes that can be configured down to the individual vertex information.

Consider the vertices used for the creation of a bufferless quad which essentially is a textured
rectangle. Every vertex has to store its position as a 2D coordinate as well as its associated
texture coordinates (also 2D). Therefore, we need 4 coordinates for each vertex, so 4 vertices
result in 16 coordinates in total.

```c++
std::array<float, 16> verts = {
	{
		-1.0f, 1.0f, 0.0f, 1.0f,  // top left
		-1.0f, -1.0f, 0.0f, 0.0f, // bottom left
		1.0f, 1.0f, 1.0f, 1.0f,   // top right
		1.0f, -1.0f, 1.0f, 0.0f   // bottom right
	}
};
```

We also need to define the layout in a `resources::VertexInputInfo` struct so that the
renderer can properly initialize the underlying vertex buffer.

```c++
resources::VertexInputInfo info{
  { resources::vertex_input_t::V2F32, resources::vertex_input_t::V2F32 },
  resources::vertex_layout_t::AOS,
  resources::vertex_primitive_t::TRIANGLE_STRIP
};
```

As seen above, we have to define 3 parameters.

1. **Vertex Input Layout**: Defines how the vertex data of each vertex is split up in the vertex shader. In this case, a vertex consists of a `vec2` for the position and a `vec2` for the texture coordinates.
1. **Vertex Buffer Layout**: Defines how the vertex data of all vertices is layed out in the whole buffer. `AOS` is *array of structs* which means that vertex data is interleaved.
1. **Vertex Primitive**: Type of primitive used for drawing the vertices.

Afterwards, vertices can be copied into a byte array which is then passed
alongside the vertex info to the `resources::MeshData` constructor.

```c++
auto const vert_data_size = verts.size() * sizeof(float);
std::vector<uint8_t> vert_data(vert_data_size);
std::memcpy(vert_data.data(), reinterpret_cast<const uint8_t *>(verts.data()), vert_data_size);

resources::MeshData mesh{ std::move(vert_data), info };
```

The resulting mesh can then be used to create a `renderer::Geometry` object from the
renderer. This will also create the vertex buffer on the GPU.

```c++
std::shared_ptr<Geometry> geom = renderer->add_mesh_geometry(mesh);
```

In addition to regular vertex meshes, the renderer also supports indexed rendering with an
index buffer. This is useful in scenarios where vertices get revisited very often, e.g. in
dense 3D meshes.

```c++
std::array<uint16_t, 16> idxs { 0, 1, 2, 1, 3, 4, 1 };
```

When indexed rendering should be used, we have to pass one additional parameter to the
`resources::VertexInputInfo` struct that specifies the layout of the index buffer:

```c++
resources::VertexInputInfo info{
  { resources::vertex_input_t::V2F32, resources::vertex_input_t::V2F32 },
  resources::vertex_layout_t::AOS,
  resources::vertex_primitive_t::TRIANGLES,
  resources::index_t::U16                   // index size -> 16 Bit unsigned integer
};
```

Furthermore, we have to copy the indices into a byte array and pass it to the mesh
constructor.

```c++
auto const idx_data_size = idxs.size() * sizeof(uint16_t);
std::vector<uint8_t> idx_data(idx_data_size);
std::memcpy(idx_data.data(), reinterpret_cast<const uint8_t *>(idxs.data()), idx_data_size);

resources::MeshData mesh{ std::move(vert_data), std::move(idx_data), info };
```

### Uniform Buffers

Uniform buffers provide a storage- and performance-efficient way to pass uniform
input values to multiple shader programs. Uniform inputs inside a uniform buffer are de-facto global
variables that can be accessed by any shader. They are best used for uniform values that
change infrequently or are the same across many shader iterations. A good use case example
are camera matrices which are at most updated once per frame and may be used in different
shader programs.

There are two ways the openage renderer can create uniform buffers. Option 1 is to
create the uniform buffer from a named uniform block in an already loaded shader program, which is
discussed below. Option 2 creates the uniform buffer from a `renderer::resources::UniformBufferInfo`
object which manually specifies the uniforms and layout of the buffer.

```c++
std::shared_ptr<ShaderProgram> shader_prog = renderer->add_shader( { vshader_src, fshader_src } );
std::shared_ptr<UniformBuffer> buffer = renderer->add_uniform_buffer(shader_prog, "unif_block");
```

To make the shader actually use the buffer, the shader's uniform block has to be bound to the
buffer first. This tells the GPU to fetch data for the uniform block from the uniform buffer
during a shader iteration.

```c++
shader_prog->bind_uniform_buffer("unif_block", buffer);
```

Setting uniform input values in the buffer works very similar to regular uniform inputs for shaders.
Instead of fetching a new uniform input object from a shader, we fetch it from the buffer object
we created.

```c++
std::shared_ptr<UniformBufferInput> buff_input = buffer->new_uniform_input(
  "color", Eigen::Vector3f{ 0.0f, 1.0f, 0.0f },
  "time", 0.0f,
  "num", 1337
);
```

This creates a `renderer::UniformBufferInput` that can be updated like their counterparts for regular
uniform inputs.

```c++
buff_input->update(
  "condition", false
);
```

An additional step is required to upload the input values to the GPU. For regular uniform inputs, this
is done automatically for each renderable in a render pass. Since uniform updates are usually updated
much less frequently or irregularly, the buffer has to be manually requested to transfer the input
values to the buffer on the GPU:

```c++
buffer->update_uniforms(buff_input);
```

Instead of creating the uniform buffer from an existing shader program, in some cases it can be beneficial
to manually define the buffer. This is particularly useful in scenarios where a shader program is not loaded
yet or if the uniform buffer is bound to multiple different shader programs.

Manually defining the buffer requires you to specify the uniform block layout as well as the name and input
type of each uniform in the buffer. From these definitions, a `renderer::resources::UniformBufferInfo`
object can be initialized, which can then be passed to the renderer to create the buffer.

```c++
resources::UBOInput view_input{ "view", resources::ubo_input_t::M4F32 };
resources::UBOInput proj_input{ "proj", resources::ubo_input_t::M4F32 };

resources::UniformBufferInfo ubo_info{
  resources::ubo_layout_t::STD140,
  { view_input, proj_input }
};

std::shared_ptr<UniformBuffer> buffer = renderer->add_uniform_buffer(ubo_info);
```

## Thread-safety
This level might or might not be threadsafe depending on the concrete backend. The OpenGL version is, in typical GL fashion, so not-threadsafe it's almost anti-threadsafe. All code must be executed sequentially on a dedicated window thread, the same one on which the window and renderer were initially created. The plan for the Vulkan version is to make it at least independent of thread-local storage and hopefully completely threadsafe.
