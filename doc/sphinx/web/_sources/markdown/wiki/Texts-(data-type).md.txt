# Strings

The String type, called `text`, is one of the primitive data types supported by nyan.

## Syntax

```
string : text = "Hello, World!"
```

Characters of the text type are surrounded by quatation marks (`""`). Texts can contain non-ASCII characters.

### Example

```
Unit():
    name : text

Spearman(Unit):
    name = "Spearman"
```

The text `name` has the value `Spearman`.

## Operators

### (Re-)Assignment '='

```
Unit():
    name : text = "Unit"

Archer(Unit):
    name = "Archer"
```

Overwrites the old value with a new one.

### Appending a string '+='

```
Spearman():
    name = "Spearman"

Emphasize<Spearman>():
    name += "!"
```

Appends the given text to the end of another text.