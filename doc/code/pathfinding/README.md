# Pathfinding

openage's pathfinding subsystem implements structures used for navigating game entities in the
game world. These structures define movement costs for a map and allow search for a path
from one coordinate in the game world to another.

Pathfinding is a subsystem of the [game simulation](/doc/code/game_simulation/README.md) where
it is primarily used for movement and placement of game entities.

1. [Architecture](#architecture)
2. [Workflow](#workflow)


## Architecture

The architecture of the pathfinder is heavily based on the article *Crowd Pathfinding and Steering*
*Using Flow Field Tiles* by Elijah Emerson (available [here](http://www.gameaipro.com/GameAIPro/GameAIPro_Chapter23_Crowd_Pathfinding_and_Steering_Using_Flow_Field_Tiles.pdf)). A core design
decision taken from this article was the usage of flow fields as the basis for the pathing algorithm.

Flow fields offer a few advantages for large group movements, i.e. the movement you typically see in
RTS games like Age of Empires. For example, they allow reusing already computed path results for
subsequent pathing requests and can also be used for smart collision avoidance. One downside
of using flow fields can be the heavy upfront cost of pathing calculations. However, the downsides
can be mitigated to some degree with caching and high-level optimizations.

The openage pathfinder is tile-based, like most flow field pathfinding implementations. Every tile
has a movement cost associated with it that represents the cost of a game entity moving on that tile.
When computing a path from A to B, the pathfinder tries to find the sequence of tiles with the cheapest
accumulated movement cost.

In openage, the pathfinding subsystem is independent from the terrain implementation in the game
simulation. Terrain data may be used to influence movement cost during gameplay, but pathfinding
can be initialized without any requirements for terrain code. By only loosely connecting these two system,
we have a much more fine-grained control that allows for better optimization of the pathfinder.
The most relevant similarity between terrain and pathfinding code is that they use the same
[coordinate systems](/doc/code/coordinate-systems.md#tiletile3). To make the distinction between
pathfinding and terrain more clear, we use the term *cells* for tiles in the pathfinder.

![UML pathfinding classes](/doc/code/images/pathfinder_architecture.svg)

The relationship between classes in the pathfinder can be seen above. `Grid` is the top-level structure
for storage of movement cost. There may be multiple grids defined, one for each movement type
used by game entities in the game simulation.

Every grid is subdivided into sectors, represented by the `Sector` class. Each sectors holds a
pointer to a `CostField` which stores the movement cost of individual cells. All sectors on the
same grid have a fixed square size that is determined by the grid. Thus, the sector size on
a grid is always consistent but different grid may utilize different sector sizes.

Sectors on a grid are connect to each other with so-called portals, see the `Portal`
class. Portals represent a passable gateway between two sectors where game entities
can pass through. As such, portals store the coordinates of the cells in each sector
where game entities can pass. `Portal` objects are created for every continuous sequence of cells
on the edges between two sectors where the cells are passable on both sides of the two sectors.
Therefore, there may be multiple portals defined for the edge between two sectors.

Additionally, each portal stores which other portals it can reach in a specific sector. The
result is a portal "graph" that can be searched separately to determine which portals
and sectors are visited for a specific pathing request. This is used in the high-level
pathfinder to preselect the sectors that flow fields need to be generated for (see the
[Workflow section](#workflow)).

The individual movement cost of each cell in a sector are recorded in a `CostField` object.
The cost of a cell can range from 1 (minimum cost) to 254 (maximum cost), while a cost of 255
makes a cell impassible. For Age of Empires, usually only the minimum and impassable cost
values are relevant. The cost field is built when the grid is first initialized and
individual cost of cells can be altered during gameplay events.

To get a path between two coordinates, the game simulation mainly interfaces with a `Pathfinder`
object. The `Pathfinder` class calls the actual pathfinding algorithms used for searching
for a path and stores references to all available grids in the pathfinding subsystems. It can receive
`PathRequest` objects that contain information about the grid that should be searched as well as
the start and target coordinates of the desired path. Found paths are returned as `Path` objects
which store the waypoint coordinates for the computed path.

Flow field calculations are controlled by an `Integrator` object, which process a cost field
from a sector as well as target coordinates to compute an `IntegrationField` and a `FlowField` object.
`IntegrationField`s are intermediary objects that store the accumulated cost of reaching
the target cell for each other cell in the field, using the cell values in the cost field as basis.
From the integration field, a `FlowField` object is created. Cells in the flow field store
movement vectors that point to their next cheapest neighbor cell in the integration field. `Path`s
may be computed from these flow field by simply following the movement vectors until the
target coordinate is reached.


## Workflow

To initiate a new search, the pathfinder receives a `PathRequest` object, e.g. from the game simulation.
Every path request contains the following information:

- ID of the grid to search
- start cell coordinates
- target cell coordinates

The actual pathfinding algorithm is split into two stages.

1. High-level portal-based search to identify the visited sectors on the grid
2. Low-level flow field-based search in the identified sectors to find the waypoints for the path

The high-level search is accomplished by utilizing the portal graph, i.e. the
connections between portals in each sector. From the portal graph, a node mesh is created
that can be searched with a graph-traversal algorithm. For this purpose, the A\* algorithm
is used. The result of the high-level search is a list of sectors and portals that are
traversed by the path.

The high-level search is mainly motivated by the need to avoid costly flow field
calculations on the whole grid. As the portal graph should already be precomputed when
a path request is made, the main influence on performance is the A\* algorithm. Given
a limited number of portals, the A\* search should overall be very cheap.

The resulting list of sectors and portals is subsequently used in the low-level flow
field calculations. More details can be found in the [field types](field_types.md) document.
As a first step, the pathfinder uses its integrator to generate
a flow field for each identified sector. Generation starts with the target sector
and ends with the start sector. Flow field results are passed through at the cells
of the identified portals to make the flow between sectors seamless.

In a second step, the pathfinder follows the movement vectors in the flow fields from
the start cell to the target cell. Waypoints are created for every direction change, so
that game entities can travel in straight lines between them. The list of waypoints
from start to target is then returned by the pathfinder via a `Path` object.
