Original algorithm
------------------

Short range pathfinder:
Don't check all possible points and vectors.

Path from (x0, y0) to (x1, y1):

Draw line from (x0, y0)->(x1, y1).

Query obstruction manager for collisions:
Obstruction manager stores 2D outline of all relevant objects,
dynamically adjusts outline sizes for the size of the moving unit.
Just stores the area and shapes on the map occupied by everything.

If obstruction manager detects no collision: unit walks the line.
Else:
find the 0. collision point (=c0), find way on outline around obstruction:
branch path at c0 into 2 new ones in an `A*` like search tree.
The new paths go to the each side of the obstruction,
each one attempts to build a new path from their end to (x1, y1).
For speedup: assemble multiple obstructions to convex hulls.
