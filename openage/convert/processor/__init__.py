# Copyright 2019-2020 the openage authors. See copying.md for legal info.

"""
Drives the conversion process for the individual games.

Every processor should have three stages (+ subroutines).
    - Pre-processor: Organize data and media from the reader into a converter
                     specific format. Also prepares API objects for
                     hardcoded stuff in the older games.
    - Processor: Translates the data and media to nyan/openage formats.
    - Post-processor: Makes (optional) changes to the converted data and
                      creates the modpacks. The modpacks will be forwarded
                      to the exporter.
"""
