terrain implementation
======================


if you ever asked yourself,
"hey, myself, how is terrain stuff implemented in openage?",
this document may enlighten your mind.


ideas
-----

You might know the infinite terrain epicness of minecraft. That's exactly what we are doing in openage.

The terrain consists of loads of small tiles, grouped in "chunks".

One chunk contains an area of (currently) 16×16 tiles. Terrain is blended by the technique described in `doc/media/blendomatic`.

(short summary:
a neighbor tile is blended onto the current one,
if neighbor.priority > current.priority:
	the neighbor tile is drawn onto the current tile,
	but before that, parts are alphamasked.
)


master terrain
--------------

this is the container of all chunks.



chunk
-----

a grouped piece of terrain.
