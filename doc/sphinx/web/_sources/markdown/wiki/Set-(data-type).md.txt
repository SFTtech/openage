# Sets

Sets are one of the complex data types supported by nyan. A set is a collection of elements that does not allow duplicates. It can hold an unlimited number of values.

## Syntax

```
set_name : set(type) = {value1, value2, ..}
```

The contents of a set are surrounded by curly brackets (`{}`). Each value is seperated by a comma (`,`).

All values must be of the same data type specified with `type`.

### Example

```
Building():
    creates : set(Unit)

Barracks(Building):
    creates = {Militia, Spearman}
```

The set `creates` has two entries: `Militia` and `Spearman`. Values inserted into the set must be of type `Unit`.

## Operators

### (Re-)Assignment '='

```
Building():
    creates : set(Unit)

Barracks(Building):
    creates = {Militia, Spearman}
```

Overwrites the old value with a new one.

### Appending values '+='

```
Barracks(Building):
    creates = {Militia, Spearman}

MesoBarracks<Barracks>():
    creates += {EagleWarrior}
```

Inserts the given values into the set.

### Removing values '-='

```
ArcheryRange(Building):
    creates = {Archer, Skirmisher, Genitour}

AllyLeaves<ArcheryRange>():
    creates -= {Genitour}
```

Removes every value that matches the values in the given set.

### Keep only specified values '&='

```
ArcheryRange(Building):
    creates = {Archer, Skirmisher, CavalryArcher, HandCannoneer}

BackToFeudalAge<ArcheryRange>():
    creates &= {Archer, Skirmisher}
```

Removes every value that **not** matches the values in the given set.