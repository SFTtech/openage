Single file conversion
======================

You can use openage to convert a single slp image from some drs archive to a
png image.

The invocation could be:

SLPs in DRS archives (for older versions of Age of Empires 1, Age of Empires 2 and SWGB):
```
python3 -m openage convert-file --drs ~/games/aoe2/Data/graphics.drs 326.slp /tmp/rofl.png
```

Terrain SLPs in DRS archives (for older versions of Age of Empires 1, Age of Empires 2 and SWGB):
```
python3 -m openage convert-file --drs ~/games/aoe2/Data/terrain.drs 15006.slp --mode terrain /tmp/rofl.png
```

Standalone SLPs (Age of Empires 1: DE and Age of Empires 2: HD):
```
python3 -m openage convert-file --palette-file ~/games/aoe2hd/Data/50500.bina 326.slp /tmp/rofl.png
```

Standalone SMXs (Age of Empires 2: DE):
```
python3 -m openage convert-file --player-palette-file ~/games/aoe2de/Data/playercolor_blue.pal --palette-file ~/games/aoe2de/Data/b_west.pal u_elite_eagle.smx /tmp/rofl.png
```

Standalone SMPs (Age of Empires 2: DE):
```
python3 -m openage convert-file --player-palette-file ~/games/aoe2de/Data/playercolor_blue.pal --palette-file ~/games/aoe2de/Data/b_west.pal u_elite_eagle.smp /tmp/rofl.png
```

Have a look at `openage/convert/singlefile.py`, this is also a simple API demo
for how to interact with the aoe files.


### Interactive Shell

You can also [browse the archives and data interactively](interactive.md).
