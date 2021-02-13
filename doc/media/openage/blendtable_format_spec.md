# Blendtable Format Specification

**Format Version:** 1

The openage blendtable format is a plaintext configuration file format for defining
a blending pattern lookup table. Using this table, the renderer selects a blending
pattern for two directly adjacent terrain textures. The blending pattern is further
defined by the [blendmask format](blendmask_format_spec.md).

The blending table is referenced by [terrain format files](terrain_format_spec.md).

All attributes start with a defined keyword followed by parameter values. Some
parameters have default values and are optional. The preferred file extension is
`.bltable`.


## Quick Reference

```
# This is a blendtable configuration file
# comments start with # and are ignored

# file version
version 1

# table definitions (n x n matrix)
blendtable [
<a> <b> <c> ...
<d> <e> <f> ...
<g> <h> <i> ...
... ... ... ...
]

# pattern definitions
pattern <pattern_id> <filename>
```


## Data Type Formatting

Type     | Example | Description
---------|---------|---------
int      | `5`     | Signed Integer
float    | `1.2`   | Float
string   | `"bla"` | String of characters enclosed by `"`
token    | `off`   | Alphanumeric predefined keyword


## Attributes

### `version`

Version of the blendtable format. Increments every time the syntax
or keywords of the format change.

Parameter  | Type   | Optional | Default value
-----------|--------|----------|--------------
version_no | int    | No       | -

**version_no**<br>
Version number of the format.


#### Example

```
version 1
```


### `blendtable`

Lookup table for blend patterns blending two adjacent terrains.

There has to be exactly one `blendtable` defined.

Parameter | Type    | Optional | Default value
----------|---------|----------|--------------
matrix    | int[][] | No       | -

**matrix**<br>
A `n`x`n` matrix containing reference IDs for blending patterns. This
must be a square matrix. Columns are separated by spaces, rows
are separated by newlines. IDs in the table must be defined in the
same file by using the `pattern` attribute.

When two terrains are adjacent, the renderer selects the blending
pattern by looking it up in this table. For that purpose, it uses
the `priority` and `blend_mode` parameters of the `[blendtable](terrain_format_spec.md#frame)`
attribute in the respective terrain format definitions.

The terrain with the higher priority uses its `blend_mode` value for
the row-index in the table, the terrain with the lower priority uses
its `blend_mode` value for the column-index. The terrain with the higher
priority is then blended onto the other terrain, using the blending pattern
defined by the ID at the index.


#### Example

```
blendtable [
0  0  0
1  2  3
23 42 1337
]
```


### `pattern`

Defines a blending pattern that is used to blend one terrain texture
into a different terrain texture that is directly adjacent. The
blending mechanism is described in more detail in the [blendmask](blendmask_format_spec.md)
format documentation.

Parameter  | Type   | Optional | Default value
-----------|--------|----------|--------------
pattern_id | int    | No       | -
filename   | string | No       | -

**pattern_id**<br>
Reference ID for the blending pattern used in this file. IDs should start at `0`.

**filename**<br>
Path to the blending pattern definition file on the filesystem.

There are two ways to specify a path: relative and absolute. Relative
paths are relative to the location of the terrain definition file. They
can only refer to image resources that are in the same modpack.

```
pattern 0 "blend0.blmask"        # blend0.blmask is in the same folder as the blendtable file
pattern 1 "./blend0.blmask"      # same as above, but more explicit
pattern 2 "media/blend1.blmask"  # blend1.blmask is in the subfolder media/
```

Absolute paths start from the (virtual) modpack root (the path where all
modpacks are installed). They always begin with `/` followed by either
a modpack identifier or a shortened modpack alias enclosed by `{}`. For
information on modpack identifiers and aliases see the [modpack](modpacks.md#alias-and-identifier)
docs.

```
pattern 0 "/{aoe2_base@openage}/blend0.blmask"  # absolute path with modpack identifier
pattern 1 "/{aoe2_base}/blend0.blmask"          # absolute path with modpack alias
```

Absolute paths are the only way to reference image resources from other
modpacks.


#### Example

```
pattern 0 "blend0.blmask"
pattern 1 "./blend3.blmask"
pattern 2 "/{aoe2_base}/blend8.blmask"
```
