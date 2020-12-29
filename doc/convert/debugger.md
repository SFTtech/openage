# Converter Semantic Debugger

The converter can generate debug info for a run. This provides
information about the contents of converter parameters
during conversion. It can help to find *semantic errors* in
the code that would be hard to discover otherwise. Syntax and
runtime errors are handled by the general logger.

## Loglevels

There are 4 loglevel to choose from. They can be manually specified using
the `--debug-info [0, 1, 2, 3]` CLI parameter when starting a run. Otherwise
the converter will use its default loglevels. These are 0 when started in `no-devmode`
and 3 when started in `devmode`.

* Level 0
    * No output
    * Default for `--no-devmode`

* Level 1
    * Converter CLI args
    * Generated info file
    * Generated manifest file
    * Logfile

* Level 2
    * Info from Level 1
    * Summaries of all stages (init, read, processor, export)
        * Game version (init)
        * Mount points (init)
        * Used `.dat` file data format (read)
        * Found languages (read)
        * Found graphic files (read)
        * Number of `ConverterObject`s and `ConverterObjectGroup`s by type (processor)
        * Number of generated data, media and metadata files by type (export)

* Level 3
    * Info from Level 2
    * Summaries for every `ConverterObjectGroup`
        * Output of helper functions (`is_XYZ()` or `get_XYZ()`)
        * Content of parameters
        * Nyan object name (if available)
    * Default for `--devmode`
