# Blendtable Format Specification

**Format Version:** 1

The openage blendtable format is a plaintext configuration file format for defining
a blending mask table. The table can be used by terrain files to blend edges other
terrains with an alpha mask. In the file, a blending lookup table and blending
masks are declared.

All attributes start with a defined keyword followed by parameter values. Some
parameters have default values and are optional. The preferred file extension is
`.bltable`.


## Quick Reference

```
# This is a blendtable configuration file
# comments start with # and are ignored

# file version
version <version_no>

# table definitions (n x n matrix)
blendtable
<a> <b> <c> ...
<d> <e> <f> ...
<g> <h> <i> ...
... ... ... ...

# selection of blendomatic borders
blendmask <blend_id> <filename>
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

Lookup table for blendmasks blending two adjacent terrains.

There has to be exactly one `blendtable` defined.

Parameter | Type    | Optional | Default value
----------|---------|----------|--------------
matrix    | int[][] | No       | -

**matrix**<br>
A nxn matrix containing reference IDs for blending masks. This
must be a square matrix. Columns are separated by spaces, rows
are separated by newlines. IDs in the table must be defined in the
same file by using the `blendmask` attribute.

When two terrains are adjacent, the renderer selects the blending
mask by looking it up in this table. For that purpose, it uses
the `priority` ans `blend_mode` parameters of the `blendtable`
attribute in the respective terrain format definitions.

The terrain with the higher priority uses its `blend_mode` value for
the row-index in the table, the terrain with the lower priority uses
its `blend_mode` value for the column-index. The terrain with the higher
priority is then blended onto the other terrain, using the blendmask
defined by the ID at the index.


#### Example

```
blendtable
0  0  0
1  2  3
23 42 1337
```


### `blendmask`

Defines a blending mask that is used to blend one terrain texture
into a different terrain texture that is directly adjacent. The
blending mechanism is described in more detail in the [blendomatic](/doc/media/blendomatic.md)
documentation.

Parameter | Type   | Optional | Default value
----------|--------|----------|--------------
blend_id  | int    | No       | -
filename  | string | No       | -

**blend_id**<br>
Reference ID for the blending mask used in this file. IDs should start at `0`.

**filename**<br>
Path to the blending mask definition file on the filesystem.

There are two ways to specify a path: relative and absolute. Relative
paths are relative to the location of the terrain definition file. They
can only refer to image resources that are in the same modpack.

```
blendmask 0 "blend0.blmask"        # blend0.blmask is in the same folder as the blendtable file
blendmask 1 "./blend0.blmask"      # same as above, but more explicit
blendmask 2 "media/blend1.blmask"  # blend1.blmask is in the subfolder media/
```

Absolute paths start from the (virtual) modpack root (the path where all
modpacks are installed). They always begin with `/` followed by either
a modpack identifier or a shortened modpack alias enclosed by `{}`. For
information on modpack identifiers and aliases see the [TODO]() docs.

```
blendmask 0 "/{aoe2_base@openage}/blend0.blmask"  # absolute path with modpack identifier
blendmask 1 "/{aoe2_base}/blend0.blmask"          # absolute path with modpack alias
```

Absolute paths are the only way to reference image resources from other
modpacks.


#### Example

```
blendmask 0 "blend0.blmask"
blendmask 1 "./blend3.blmask"
blendmask 2 "/{aoe2_base}/blend8.blmask"
```
