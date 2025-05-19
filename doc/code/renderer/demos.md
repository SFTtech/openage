# Interactive Demos & Stresstests

openage builds contain several *interactive* renderer tech demo entrypoints that show of specific features.
These demos are also useful for learning the renderer API and for testing new functionality. In addition to
the demos, there are also stresstests that are used to test the performance of the renderer.
The source code for renderer demos and stresstests is located in [`libopenage/renderer/demo`](/libopenage/renderer/demo/).

This documents describes the purpose of each demo and contains instructions on how to interact with them.

1. [Demos](#demos)
   1. [Demo 0](#demo-0)

## Demos

### Demo 0

This demo shows the creation of a minmal renderer setup and the rendering of a simple mesh.

The demo initializes a GUI application, a window, a renderer object, and a render pass.
It then loads a shader program and creates a single mesh object which is then rendered to the screen
using the shader program.

The demo mostly follows the steps described in the [Level 1 Renderer - Basic Usage](level1.md#basic-usage)
documentation.

```bash
cd bin && ./run test --demo renderer.tests.renderer_demo 0
```

**Result:**

![Demo 0](/doc/code/renderer/images/demo_0.png)


### Demo 1

This demo shows how simple *textured* meshes can be created and rendered. It also demonstrates
how to interact with the window and the renderer using window callbacks.

```bash
cd bin && ./run test --demo renderer.tests.renderer_demo 1
```

**Controls:**

- <kbd>LMB</kbd>: Click on the textured meshes to get a debug message with the object ID.

**Result:**

![Demo 1](/doc/code/renderer/images/demo_1.png)


### Demo 2

In this demo, we show how animation and texture metadata files are parsed and used to
load and render the correct textures and animations for a mesh.

```bash
cd bin && ./run test --demo renderer.tests.renderer_demo 2
```

**Controls:**

- <kbd>LMB</kbd>: Click on the sprite to get a debug message with the object ID.
- <kbd>←</kbd>: Go back one frame in the animation.
- <kbd>→</kbd>: Advance the animation by one frame.

**Result:**

![Demo 2](/doc/code/renderer/images/demo_2.png)


### Demo 3

This demo shows a minimal setup for the [Level 2 Renderer](level2.md) and how to render objects
with it. The demo also introduces the camera system and how to interact with it.

```bash
cd bin && ./run test --demo renderer.tests.renderer_demo 3
```

**Controls:**

- <kbd>W</kbd>, <kbd>A</kbd>, <kbd>S</kbd>, <kbd>D</kbd>: Move the camera in the scene.
- `Mouse Wheel`: Zoom in and out.

**Result:**

![Demo 3](/doc/code/renderer/images/demo_3.png)


### Demo 4

This demos shows how animation frame timing works and how to control the animation speed
with the engine's internal clock.

```bash
cd bin && ./run test --demo renderer.tests.renderer_demo 4
```

**Controls:**

- <kbd>Space</kbd>: Pause/resume the clock.
- <kbd>+</kbd>, <kbd>-</kbd>: Increase/decrease the simulation speed.
- <kbd>Return</kbd>: Toggle between real time and simulation time.

**Result:**

![Demo 4](/doc/code/renderer/images/demo_4.png)


### Demo 5

This demo shows how to create [uniform buffers](level1.md#uniform-buffers) and how to use them to pass data to shaders.
Additionally, uniform buffer usage for the camera system is demonstrated.

```bash
cd bin && ./run test --demo renderer.tests.renderer_demo 5
```

**Controls:**

- <kbd>W</kbd>, <kbd>A</kbd>, <kbd>S</kbd>, <kbd>D</kbd>: Move the camera in the scene.

**Result:**

![Demo 5](/doc/code/renderer/images/demo_5.png)


### Demo 6

This demo shows how to use [frustum culling](level2.md#frustum-culling) in the renderer.

```bash
cd bin && ./run test --demo renderer.tests.renderer_demo 6
```

**Controls:**

- <kbd>W</kbd>, <kbd>A</kbd>, <kbd>S</kbd>, <kbd>D</kbd>: Move the camera in the scene.

**Result:**

![Demo 6](/doc/code/renderer/images/demo_6.png)


### Demo 7

This demo shows how to use [shader templating](level1.md#shader-templates) in the renderer.

```bash
cd bin && ./run test --demo renderer.tests.renderer_demo 7
```

**Result:**

![Demo 7](/doc/code/renderer/images/demo_7.mp4)


## Stresstests

### Stresstest 0

This stresstest tests the performance when rendering an increasingly larger number of objects.

```bash
cd bin && ./run test --demo renderer.tests.renderer_stresstest 0
```

**Result:**

![Stresstest 0](/doc/code/renderer/images/stresstest_0.png)

### Stresstest 1

This stresstest tests the performance when [frustum culling](level2.md#frustum-culling) is enabled and an increasingly larger
number of objects is rendered on the screen.

```bash
cd bin && ./run test --demo renderer.tests.renderer_stresstest 1
```

**Result:**

![Stresstest 1](/doc/code/renderer/images/stresstest_1.png)
