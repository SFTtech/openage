# Boolean

Boolean values are one of the primitive data types supported by nyan. A boolean can only be one of two possible values: `True` or `False`.

## Syntax

```
decision : bool = True
```

### Example

```
Unit():
    convertible : bool

Spearman(Unit):
    convertible = True

Ram(Unit):
    convertible = False
```

The boolean value `convertible` of the object `Spearman` has the value `True`, while the boolean `convertible` of the object `Ram` has the value `False`.

## Operators

### (Re-)Assignment '='

```
Unit():
    convertible : bool = False

Archer(Unit):
    convertible = True
```

Overwrites the old value with a new one.

### Logical AND '&='

```
Ram():
    convertible = False

Conformity<Ram>():
    convertible &= True
```

If **both** the given and the original value are `True`, the resulting value will also be `True`, else the new value will be `False`.

### Logical OR '|='

```
Ram():
    convertible = False

Conformity<Ram>():
    convertible |= True
```

If **at least one** of the two values is `True` - i.e. the given value, the original value or both -, the resulting value will be `True`. Only if both values are `False`, the new value will also be `False`.