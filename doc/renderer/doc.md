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

### Shaders

<NOTE: this is an unorganized braindump, i'll rewrite it later>

These are more involved than resources, since each shader also carries a set of uniforms.

A shader program takes in uniform values and vertex inputs and produces a number of bitmaps written into its render targets. Every shader program is then a function of the form $S:P\times\prod\limits^{n_u} U_i\times\left(\prod\limits^{n_a}V_j\right)^n\rightarrow\prod\limits^{n_t}B_k$, where $P$ is the primitive type, $\{U_i\}$, $\{V_j\}$, $\{B_k\}$ are indexed families of uniform, vertex attribute and bitmap types, respectively and the shader takes in $n_u$ uniforms, $n$ vertices with $n_a$ attributes per vertex and outputs to $n_t$ render targets.

All of these inputs are contained in the $Renderable$ type. There is a catch here, since shaders are stateful, arguments are preserved between calls.

Keep in mind that the output bitmap is not necessarily what the final output in a render target will be, since multiple draw calls get combined into a single bitmap and the initial value of the target also has an effect.

With this premise, we can create a renderer interface generic over low-level APIs and usage (although optimized for openage's purposes). The only renderables are sets of values to be passed to shader programs - they may or may not include geometry, textures, matrices, etc.

Our $ShaderInput$ class conceptually has almost the same member types as the types in the domain of the corresponding shader program function, with some small differences. For example, OpenGL shaders have implicit vertex attributes like $gl\_VertexID$ which we do not valuate manually.

For the most part, these types are not know statically, so $ShaderInput$ has to simulate them internally with dynamic dispatch while providing an external interface that acts as if its members are the same as the inputs to the shader.

OTOH, when it is the case that they are know statically, we can generate the appropriate headers from the shader code for better performance (in the future).

Figure out the interface for $ShaderInput$.

Parametrize $ShaderInput$ over a $Shader$ type. Have one $DynamicShader$ for all shaders loaded at runtime and some $StaticShaderForSpecificThing$ classes for shaders known beforehand. Make $DynamicShader$ compile for all operations and $StaticShader$.. only have operations it supports implemented, making it check them at compile-time.

```c++
template<typename S>
class ShaderInput {
  template<unsigned int U, typename T>
  void set_unif(T val) {
    // always compiles for dynamic shader,
    // only compiles for valid U and T for static shader
    (instanceof S).set_unif<U, T>(val);
  }
};

template<unsigned int N>
void func() {
    static_assert(N != N, "Invalid shader uniform name.");
}

template<>
void func<COMPILE_TIME_CRC32_STR("val1")>() {
    cout << "val1" << endl;
    return;
}

template<>
void func<COMPILE_TIME_CRC32_STR("val2")>() {
    cout << "val2" << endl;
    return;
}
```

How to deal with drawing the scene?
We want stateless drawing, but pure stateless incurs some overhead. We can probably do with some mix of sharing scene between renderer and user. Keep a Scene object containing renderables (just shader valuations) and pass it to the render method.
What shoud Scene contain?
```c++
struct Scene {
    std::vector<ObjectId>;
    OR
    std::vector<std::shared_ptr<Object>>;
    SceneSettings...
        alpha blending, ztest, all of that in Object
        is there anything global?
        From Unity3D:
            far, near plane
            default depth
            projection type
            occlusion culling method
            HDR
            AA styles
            how to deal with unit picking? probably have to make it a separate render pass
            that renders unit ids into a buffer. read it on CPU or GPU? GPU faster, but is it possible?
            general render-pass related things; have objects for render passes?
};
```
alternatively call render_pass(..) for each pass rather than call render(OBJ) once where OBJ contains info about render passes. might be nicer since render pass info can be recovered. problem - have to sync CPU/GPU to wait for command buffer execution finish
QUESTION:
    share ownership of scene data at all times or give views to the user that are only accessible when not rendering?

### Usage
Sample usage:

```c++
Window window("title");
auto renderer = opengl::GlRenderer(window.get_context());

resources::TextureData tex_data("/path.tex");
std::unique_ptr<Texture> tex = renderer->add_texture(tex_data);

resources::ShaderSource vsrc = resources::ShaderSource::read_from_file("/path.vert", resources::shader_t::glsl_vertex);
resources::ShaderSource fsrc = resources::ShaderSource::read_from_file("/path.frag", resources::shader_t::glsl_fragment);
                                                                       
std::unique_ptr<ShaderProgram> prog = renderer->add_shader( { vsrc, fsrc } );

auto input = prog->new_uniform_input(
  "color", { 0.0f, 1.0f, 0.0f },
  "time", 0.0f,
  "num", 1337
);

RenderPass pass {
  { {
    input,
    new Geometry(geometry_t::quad),
    true,
    true,
    true,
    true,
  } }, // list of renderables
  renderer->get_framebuffer_target(),
  1.0f,
  8,
};

renderer->render(pass);
```

## Level 2:
On top of that renderer, we build a level 2 graphics subsystem. It has an API that is actually specific to openage, and is threadsafe. The level-2 renderer calls the level 1 renderer and updates it to match the game state.
