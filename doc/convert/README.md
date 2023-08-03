# Convert Script

The convert script transforms media files to openage-compatible
formats, and generates code for parsing the generated files. It is written
in **Python**, **Cython** and contains **C++ extensions**.

As we are using the media assets of the original game, the input format is
pre-existing and we have to deal with it.

Unfortunately the original asset binary formats are a bit "special",
so we need to convert the files to a format that is less special, and more
usable. This is what the convert script does.


The convert script is divided into two usage modes: *singlefile media conversion* and
*modpack conversion*.

## Singlefile Media Conversion

This converts graphics and sounds to open/compatible formats. Graphics are
exported to PNG files. If the graphics file is an animation with multiple
sprites, the converter will automatically pack all of them into a spritesheet.
Sounds are exported to OPUS files.

[See here](convert_single_file.md) for usage examples of the singlefile conversion.


## Modpack Conversion

Modpack conversion takes an original game installation and converts its data
and media files into an [openage modpack](/doc/media/openage/modpacks.md).
The converted game can then be run inside openage.
