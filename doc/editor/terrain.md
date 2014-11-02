# Terrain editor

## General
The terrain editor is in fact a few toolboxes and an engine opengl context. The toolboxes contain terrain tiles and objects to be placed on the map. An object is hereby placed by a terrain brush, which size is adjustable. Imagine Bob Ross painting trees. Exactly like that.

## Toolboxes
*Note: As of writing, the engine has no concept of height. Therefore, it will be ignored here as well.*
The terrain tool boxes may be divided in subgroups with descriptive names. Also, there should be distinguished between game objects (such as trees or buildings) and the flat terrain tiles (like grass, water or dirt).

Note also that not every object is valid on every terrain tile. For example, a footbridge needs water underneath and a non-water, accessible tile on the connecting side.

The toolboxes should be filled with preview images of the property they represent. A informative description may be present, but must be short. I believe that it is easier to find the correct visual by simply clicking on what i want to paint rather than having to figure out what $creator meant with $description.

## Painting
The painting area is where terrain manipulation happens. It is essentially an opengl sink for the engine, but has a grid painted over it. Once something from the toolboxes is selected, moving the mouse over the painting area outlines the size (n tiles diameter, n*m tiles wide) and shape of the current brush (square, circle, ...). Clicking (and holding while moving) paints all tiles within the current bounds with the selected terrain tool.

## UI Layout
See [terrain-ui.txt](terrain-ui.txt) for a layout draft.
