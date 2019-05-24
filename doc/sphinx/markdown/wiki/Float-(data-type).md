# Floats

Floats are one of the primitive data types supported by nyan. They are used for representing fractions and rational numbers. Floats can be positive, negative or infinite (noted as `inf`).

## Syntax

```
positive_float : float = 2.05
negative_float : float = -15.234
infinite_float : float = inf
```

### Example

```
Unit():
    speed : float

Archer(Unit):
    speed = 2.03
```

The float `speed` has a value of `2.03`.

## Operators

### (Re-)Assignment '='

```
Unit():
    speed : float = 1.0

Archer(Unit):
    speed = 0.9
```

Overwrites the old value with a new one.

### Addition '+='

```
Archer(Unit):
    speed = 1.05

FasterArchers<Archer>():
    speed += 0.56
```

Adds a given value to the float.

### Substraction '-='

```
Archer(Unit):
    speed = 1.05

SlowerArchers<Archer>():
    speed -= 0.56
```

Substracts a given value from the float.

### Multiplication '*='

```
Archer(Unit):
    speed = 1.05

SpeedyArchers<Archer>():
    speed *= 2.5
```

Multiplies a float by another float or an integer.

### Division '/='

```
Archer(Unit):
    speed = 1.05

SnailArchers<Archer>():
    speed /= 1.5
```

Divides a float by another float or an integer.