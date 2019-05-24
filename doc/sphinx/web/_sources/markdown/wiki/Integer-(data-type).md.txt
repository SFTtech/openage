# Integers

Integers are one of the primitive data types supported by nyan. They can be positive, negative or infinite (noted as `inf`). 

## Syntax

```
positive_number : int = 1
negative_number : int = -5
infinite : int = inf
```

### Example

```
Unit():
    hp : int

Archer(Unit):
    hp = 45
```

The integer `hp` has a value of `45`.

## Operators

### (Re-)Assignment '='

```
Unit():
    hp : int = 10

Archer(Unit):
    hp = 40
```

Overwrites the old value with a new one.

### Addition '+='

```
Archer(Unit):
    hp = 45

BetterArcher<Archer>():
    hp += 10
```

Adds a given value to the integer.

### Substraction '-='

```
Archer(Unit):
    hp = 45

WorseArcher<Archer>():
    hp -= 10
```

Subtracts a given value from the integer.

### Multiplication '*='

```
Archer(Unit):
    hp = 45

SuperArcher<Archer>():
    hp *= 1.8
```

Multiplies an integer by another integer or a float.

### Division '/='

```
Archer(Unit):
    hp = 45

WeakArcher<Archer>():
    hp /= 1.2
```

Divides an integer by another non-zero integer or non-zero float.