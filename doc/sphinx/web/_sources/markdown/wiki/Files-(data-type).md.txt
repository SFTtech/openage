# Files

Files are one of the primitive types supported by nyan. A file holds a file descriptor with a path to any file as a value. Paths are always relative to the location of the nyan object which contains the `file` member.

## Syntax

```
sample_file : file = "./folder1/sample.png"
```

The `file` data typpe uses the same syntax as the `text` type. The path is surrounded by quotation marks (`""`) and uses the UNIX-like notation of a forward slash (`/`) for path separators. 

### Example

```
Unit():
    hud_icon : file

Archer(Unit):
    hud_icon = "archer_icon.png"
```

The file `hud_icon` has a value of `archer_icon.png`. Because paths are relative, we also know that `archer_icon.png` should be located in the same folder as the nyan object declaration.

## Operators

### (Re-)Assignment '='

```
Archer():
    hud_icon = "archer_icon.png"

UpgradeToCrossbowman<Archer>():
    hud_icon = "crossbow_icon.png"
```

Overwrites the old value with a new one.