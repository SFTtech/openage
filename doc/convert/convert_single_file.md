Single file conversion
======================

You can use openage to convert a single slp image from some drs archive to a
png image.

The invocation could be:

```
python3 -m openage convert-file ~/games/aoe2/aoe2/Data/graphics.drs 326.slp /tmp/rofl.png
```

Have a look at `openage/convert/singlefile.py`, this is also a simple API demo
for how to interact with the aoe files.



### Interactive Shell

You can also [browse the archives and data interactively](interactive.md).
