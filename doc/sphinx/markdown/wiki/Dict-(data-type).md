# Dicts

Dicts are one the the complex data types supported by nyan. They are also known as *associative arrays* or *maps* in other programming languages. A dict behaves like a list with the speciality that every inserted value is mapped to a *key*. Any non-abstract `Nyan::Object` can be used as a key. Referencing a key will return the associated value. Dicts can store an unlimited number of key-value pairs.

*Dicts are currently not implemented in nyan.*

## Syntax

```
dict_name : dict(keyType, valueType) = {Key1: value1, Key2: value2, ..}
```

The contents of a dict are surrounded by curly brackets (`{}`). A key and its associated value are separated by a colon (`:`). Each key-value pair is seperated by a comma (`,`).

Per definition all keys must use the same type of `Nyan::Object` (inherited objects included). You can't use the same key twice. Values also have to be of the same type as other values, but unlike in sets there is no restriction on how often a value can occur.

### Example

```
Unit():
    cost : dict(Resource, int)

Archer(Unit):
    cost = {Wood: 20, Gold: 45}
```

The dict `cost` has two entries: `Wood: 20` and `Gold: 45`. The first entry uses the key `Wood` which has the associated value `20`. The second entry's key is `Gold` which has the mapped value of `45`. This dict can store key-value pairs where the key has the type `Resource` and the value is of the type `Integer`. 

## Operators

### (Re-)Assignment '='

```
Unit():
    cost : dict(Resource, int)

Archer(Unit):
    cost = {Wood: 20, Gold: 45}
```

Overwrites the old value with a new one.

### Addition '+='

```
Archer(Unit):
    cost = {Wood: 20, Gold: 45}

MoreCost<Archer>():
    cost += {Wood: 5}
```

Adds the given value to the value with the same key.

### Substraction '-='

```
Archer(Unit):
    cost = {Wood: 20, Gold: 45}

MoreCost<Archer>():
    cost -= {Wood: 5}
```

Substracts the given value from the value with the same key.

### Multiplication '*='

```
Archer(Unit):
    cost = {Wood: 20, Gold: 45}

MoreCost<Archer>():
    cost *= {Wood: 5}
```

Multiplies the given value with the value with the same key.

### Division '/='

```
Archer(Unit):
    cost = {Wood: 20, Gold: 45}

MoreCost<Archer>():
    cost /= {Wood: 5}
```

Divides the original value by the given value when the keys are the same.

### Appending a dict '|='

```
Archer(Unit):
    cost = {Wood: 20, Gold: 45}

MoreCost<Archer>():
    cost |= {Food: 10}
```

Adds the given key-value pairs to the dict. If a key is already present in a dict, the original key-value pair is kept.

### Deleting keys '?='

```
Archer(Unit):
    cost = {Wood: 20, Gold: 45}

MoreCost<Archer>():
    cost ?= {Wood, Food}
```

Removes every key-value pair where the key matches the keys in the given set.

### Keep only specified keys '&='

```
Archer(Unit):
    cost = {Wood: 20, Gold: 45}

MoreCost<Archer>():
    cost &= {Wood}
```

Removes every key-value pair where the key **not** matches the keys in the given set.