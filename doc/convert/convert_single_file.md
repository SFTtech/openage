# Singlefile conversion

You can use openage to convert a single graphics file to PNG or sound file to OPUS. The
asset can also be unpacked from a DRS archive.

The invocation could be:

SLPs in DRS archives (for older versions of Age of Empires 1, Age of Empires 2 and SWGB):
```
python3 -m openage convert-file --palettes-path ~/games/aoe2/Data/ --drs ~/games/aoe2/Data/graphics.drs 326.slp /tmp/rofl.png
```

Standalone SLPs (Age of Empires 1: DE and Age of Empires 2: HD):
```
python3 -m openage convert-file --palettes-path ~/games/aoede/Assets/Palettes 326.slp /tmp/rofl.png
```

Standalone SLDs (Age of Empires 2: DE):
```
python3 -m openage convert-file --palettes-path ~/games/aoe2de/Data/ u_elite_eagle.sld /tmp/rofl.png
```

Standalone SMXs (Age of Empires 2: DE):
```
python3 -m openage convert-file --palettes-path ~/games/aoe2de/Data/ u_elite_eagle.smx /tmp/rofl.png
```

Standalone SMPs (Age of Empires 2: DE):
```
python3 -m openage convert-file --palettes-path ~/games/aoe2de/resources/_common/palettes u_elite_eagle.smp /tmp/rofl.png
```

WAVs in DRS archives (for older versions of Age of Empires 1, Age of Empires 2 and SWGB):
```
python3 -m openage convert-file --drs ~/games/aoe2/Data/sounds.drs 123.wav /tmp/rofl.opus
```

Standalone WAVs (Age of Empires 2: HD Edition):
```
python3 -m openage convert-file ~/games/aoe2/resources/123.wav /tmp/rofl.opus
```

Have a look at `openage/convert/singlefile.py`, this is also a simple API demo
for how to interact with the aoe files.


### Interactive Shell

You can also [browse the archives and data interactively](interactive.md).
