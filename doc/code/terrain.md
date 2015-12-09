Terrain implementation
======================


If you ever asked yourself,
"hey, myself, how is terrain stuff implemented in openage?",
This document may enlighten your mind.


Ideas
-----

You might know the infinite terrain epicness of Minecraft. That's exactly what we are doing in openage.

The terrain consists of loads of small tiles, grouped in "chunks".

One chunk contains an area of (currently) 16×16 tiles. Terrain is blended by the technique described in [doc/media/blendomatic.md](/doc/media/blendomatic.md).

(short summary:
A neighbor tile is blended onto the current one,
if neighbor.priority > current.priority:
	the neighbor tile is drawn onto the current tile,
	but before that, parts are alphamasked.
)


Master terrain
--------------

This is the container of all chunks.



Chunk
-----

A grouped piece of terrain.
