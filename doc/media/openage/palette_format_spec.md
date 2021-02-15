# Palette Format Specification

**Format Version:** 1

The openage palette format is a plaintext configuration file format for defining
colour palettes. It tells the openage renderer a table of predefined RGBA colour values
that can be used for special pixels such as player colour pixels.

All attributes start with a defined keyword followed by parameter values. Some
parameters have default values and are optional. The preferred file extension is
`.opal`.


## Quick Reference

```
# This is a palette configuration file
# comments start with # and are ignored

# file version
version 1

# number of entries in the colour table
entries <count>

# Colour values
colours [
<int> <int> <int> <int>
...
]
```


## Data Type Formatting

Type     | Example | Description
---------|---------|---------
int      | `5`     | Signed Integer


## Attributes

### `version`

Version of the palette format. Increments every time the syntax
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


### `entries`

Defines the number of colour values in the palette.
There has to be exactly one `entries` defined.

Parameter  | Type   | Optional | Default value
-----------|--------|----------|--------------
count      | int    | No       | -

**count**<br>
Number of colour values in the palette.


#### Example

```
entries 256
```


### `colours`

Defines the array containing the colour values.
There has to be exactly one `colours` defined.

Parameter  | Type    | Optional | Default value
-----------|---------|----------|--------------
values     | int[]   | No       | -

**values**<br>
An array of integer values in the range of `0` to `255`. Every palette
entry is a 4-tuple of integers, with each value representing a channel
byte value for an *RGBA* color.

The parameter array must contain exactly `count * 4` values where `count`
is the parameter value from the `entries` attribute.


#### Example

```
colours [
255 255 255 0
4 3 2 1
]
```
