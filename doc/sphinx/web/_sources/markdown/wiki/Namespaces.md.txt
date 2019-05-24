# Namespaces

So far our examples only covered a few objects at a time. In practice, games or ambitious mods can require hundreds or even thousands of different objects. Ideally these should not be put into a single file, but organized in several files which are sorted into a sane folder structure themselves. That begs the question how one can reference an object that is declared in another file somewhere else in the modpack. As you probably guessed, the answer is *namespaces*. 

## Spreading objects across files

The engine will automatically assume that a file with the ending `.nyan` (nyan file) contains nyan objects. Modders can choose freely how they want to organize the folder structure of their modpack and which objects are put in which nyan files. Each nyan file creates a *namespace* that is derived from the file's path and name. Objects in the file are all part of the same namespace.

```
tentaclemod
|--buildings
|--units
   |--monster.nyan
      ...

# filename: tentaclemod/units/monster.nyan
# namespace: tentaclemod.units.monster
TentacleMonster():
    hp = 5
    ...

TentacleUpgrade<TentacleMonster>():
    hp += 15
    ...

MonsterHunter():
    hp = 10
    ...
```

In the above example the filepath is `tentaclemod/units/monster.nyan` which translates to the namespace `tentaclemod.units.monster`. The objects `TentacleMonster`, `TentacleUpgrade` and `MonsterHunter` are part of this namespace.

### Fully qualified object name

Objects can be referenced from anywhere using the *fully qualified object name* (fqon). The fqon is a combination of the namespace and the object name. Given the previous example, the object `TentacleMonster` in `monster.nyan` has the fqon `tentaclemod.units.monster.TentacleMonster`. This also allows referencing objects in other mods:

```
ageofempires2
|--buildings
|--units
   |--spearman.nyan

# filename: ageofempires2/units/spearman.nyan
# namespace: ageofempires2.units.spearman

Spearman():
    hp = 40
    speed = 2.0
    ...
```
```
spearmod
|--units
   |--spearman.nyan
      ...

# filename: spearmod/units/spearman.nyan
# namespace: spearmod.units.spearman

# This object inhherits from Spearman
# in namespace ageofempires2.units.spearman
SuperSpearman(ageofempires2.units.spearman.Spearman):
    hp += 20
    speed += 0.5
    ...
```

`SuperSpearman` inherits from `Spearman` in another mod. In this case the mod is the Age of Empires 2 base game, but user mods can also reference each other.

Because an fqon must be unique, objects in the same namespace must not have the same object name. However, objects in different namespaces are allowed to share the same object name.

```
ageofempires2
|--buildings
|--units
   |--spearman.nyan

# filename: ageofempires2/units/spearman.nyan
# namespace: ageofempires2.units.spearman

Spearman():
    hp = 40
    speed = 2.0
    ...
```
```
japaneseunitsmod
|--units
   |--spearman.nyan
      ...

# filename: japaneseunitsmod/units/spearman.nyan
# namespace: japaneseunitsmod.units.spearman

# Different spearman
Spearman():
    hp = 50
    speed = 1.5
    ...
```

Above are two objects with the object name `Spearman`. Even though they are share the same name, their fqon stays unique. The spearman from Age of Empires 2 is referenced with the fqon `ageofempires2.units.spearman.Spearman`, while the one from the Japanese Units Mod has the fqon `japaneseunitsmod.units.spearman.Spearman`.

## Importing

fqons tend to get very long depending on the file names and folder structure. To combat this problem, nyan allows to *import* a namespace under an alias.

```
ageofempires2
|--buildings
|--units
   |--spearman.nyan

Spearman():
    hp = 40
    speed = 2.0
    ...
```
```
spearmod
|--units
   |--spearman.nyan
      ...

import ageofempires2.units.spearman as basegame

# 'basegame' is an alias for namespace 'ageofempires2.units.spearman'
SuperSpearman(basegame.Spearman):
    hp += 20
    speed += 0.5
    ...
```