# Orderes Sets

Ordered sets are one of the complex data types supported by nyan. An ordered set behaves like a set, but also stores the order in which the values where put in. This is useful for those cases where a **guaranteed order** is important, e.g. for the application of patches and inheriting objects.

## Syntax

```
oset_name : orderedset(type) = o{value1, value2, ..}
```

The contents of an ordered set are surrounded by curly brackets with an `o` in the front (`o{}`). Each value is seperated by a comma (`,`).

All values must be of the same data type specified with `type`. Just like in sets, ordered sets do not hold duplicates of values.

### Example

```
Tech():
    updates : orderedset(Patch)

ResetArmor<Infantry>():
    m_armor = 1
    p_armor = 0

PierceArmor<Infantry>():
    p_armor += 1

ArmorTech(Tech):
    updates = o{ResetArmor, PierceArmor}
```

The ordered set `updates` has two entries: `ResetArmor` and `PierceArmor`. From the decleration in `Tech()` we know that `updates` allows values of type `Patch` to be inserted. The patch `ResetArmor` comes before `PierceArmor` and will therefore be applied first when a technology is researched. If the patches were ordered the other way around, the outcome would be different.

Result for `updates = o{ResetArmor, PierceArmor}`:

```
m_armor = 1
p_armor = 1
```

Result for `updates = o{PierceArmor, ResetArmor}`:

```
m_armor = 1
p_armor = 0
```

## Operators

### (Re-)Assignment '='

```
Tech():
    updates : orderedset(Patch)

ArmorTech(Tech):
    updates = o{ResetArmor, PierceArmor}
```

Overwrites the old value with a new one.

### Appending values '+='

```
ArmorTech(Tech):
    updates = o{ResetArmor, PierceArmor}

MoreMelee<ArmorTech>():
    updates += o{MeleeArmor}
```

Appends the given values to the end the ordered set.

### Removing values '-='

```
ArmorTech(Tech):
    updates = o{ResetArmor, PierceArmor}

RemoveReset<ArmorTech>():
    updates -= o{ResetArmor}
```

Removes every value that matches the values in the given set.

For this operation you can also pass a normal set:

```
ArmorTech(Tech):
    updates = o{ResetArmor, PierceArmor}

RemoveReset<ArmorTech>():
    updates -= {ResetArmor}
```

### Keep only specified values '&='

```
ArmorTech(Tech):
    updates = o{ResetArmor, PierceArmor}

OnlyPierce<ArmorTech>():
    updates &= o{PierceArmor}
```

Removes every value that **not** matches the values in the given set.

For this operation you can also pass a normal set:

```
ArmorTech(Tech):
    updates = o{ResetArmor, PierceArmor}

OnlyPierce<ArmorTech>():
    updates &= {PierceArmor}
```