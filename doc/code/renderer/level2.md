## Level 2

High-level renderer for transforming data from the gamestate to render objects for the level 1 renderer.

### Stages

Every stage has its own subrenderer that manages a `RenderPass` from the level 1 renderer and updates it with `Renderable`s created using update information from the gamestate. Stages also store the vertex and fragment shaders used for drawing the renderable objects.

There are currently 5 stages in the level 2 rendering pipeline:

1. `SkyboxRenderer`: Draws the background behind the terrain (as a single color).
1. `TerrainRenderer`: Draws the terrain. Terrains are handled as textured 3D meshes.
1. `WorldRenderer`: Draws animations and sprites for units/buildings and other 2D ingame objects.
1. `GuiRenderer`: Draws the GUI overlay. The drawing part in this stage is actually done by Qt, while the level 1 renderer only provides the framebuffer.
1. `ScreenRenderer`: Alpha composites the framebuffer data of previous stages and draws them onto the screen (i.e. it overlays the outputs from the other stages).

With the exception of the `ScreenRenderer`, all stages are independent from each other, i.e. the order in which they are rendered each frame does not matter. To render the current frame for a stage, call its `update()` method. The `ScreenRenderer` must be rendered last as it combines the output of all other stages into the result shown on screen.

### Updating Render Stages from the Gamestate

Gamestate (game simulation) and level 2 rendering are largely decoupled, i.e. communication between these components only happens via a few dedicated connection interfaces in the codebase. Furthermore, the information flow between gamestate and renderer is strictly one-way (always gamestate -> renderer). In practice, this means that the gamestate constantly updates the level 2 renderer on what it should display. The renderer only acts on what it receives from the gamestate and does not actively request or send information to the gamestate. We mainly do this for thread-safety reasons as gamestate and renderer live in two different threads at runtime.

There are two render stages that can receive updates from the gamestate: `WorldRenderer` and `TerrainRenderer`. You can see how updates are propagated from the gamestate to these stages here:

**WorldRenderer**                             | **TerrainRenderer**
----------------------------------------------|------------------------------------------------
![world drawing](images/world_renderer.svg)   | ![terrain drawing](images/terrain_renderer.svg)

As you can see, the workflow for both render stages is roughly the same and involves these components:
  - **game entity**: Object in the game world managed by the gamestate, e.g. a unit or a building.
  - **render entity**: Connector object between game entity and the render stage. The game entity sends updates to the render entity to communicate what should be displyed. Furthermore, the render entity transforms said updates into something usable by the renderer, e.g. loading the texture for a texture ID. Render entities also contain the critical path between the gamestate and renderer thread boundaries.
  - **render object(s)**: Store the current render state for a `Renderable` created for a render entity, e.g. uniforms, meshes or IDs. There can be a variable number of these objects depending on the complexity of the render state. For example, the `TerrainRenderer` stage has separate objects for the mesh (`TerrainMesh`) and model (`TerrainModel`) for its terrain. Render objects poll the render entity for updates every frame and may change the render state when updated.
  - **(sub-)renderer**: Manages all render objects for the render stage, i.e. by polling them for updates and creating level 1 `Renderable`s from their render state which are added to the stage's render pass. New render entities are also registered at the subrenderer. Upon registration, the subrenderer creates the necessary render objects and attaches them to the render entity.
  - **render factory**: Factory in the gamestate for creating and registering new render entities. Game entities can request render entities from the factory, which then also registers the render entity at the corresponding subrenderer. The render factory lives in the gamestate thread, but crosses thread boundaries when registering new render entities. Therefore, it is also part of the critical path.
