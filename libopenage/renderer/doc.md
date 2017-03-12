The renderer implementation:



First things first, we might want to support multiple APIs. For now just OpenGL, but maybe Vulkan or some others.
We want to abstract over these, but this can't unfortunately be done at the level of graphics primitives like
textures, buffers, etc. Well, it can, but it introduces unnecessary complexity and possible overhead. That is because
the next-gen (Vulkan, Metal, DX12) APIs are vastly different from the old ones - most importantly, they're bindless,
so something like a Vulkan context (GL notion) doesn't even make sense. We therefore choose to abstract on the higher
level of things-to-draw.

This is the level-1 renderer. We want a separate rendering thread, so the api is thread-safe. It works similarly to
the Unity engine. The first part is resources. The user can submit resources to be uploaded to the gpu and receives
a numeric handle that identifies the uploaded resource. Resources can be added, updated and removed. Currently
supported resource types: mesh, texture.

Then, we have to deal with shaders. They are more involved than resources, since each shader also carries a set of
uniforms.

TODO describe MaterialValuations

On top of that renderer, be build a level-2 renderer. This one has an api that is actually specific to openage, and isn't
threadsafe. That way it can share data with the simulation code and others. The level-2 renderer calls the level-1 renderer
and updates it to match the game state.

Namespaces:

openage::renderer - everything related to rendering
openage::renderer::opengl - rendering using OpenGL
openage::renderer::vulkan - rendering using Vulkan

every successive namespace is an actual directory and all its classes are contained there


