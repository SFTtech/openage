#!/usr/bin/python3
import argparse

import datafile
import mediafile
from util import set_verbosity

#these sections of the dat file will be exported
default_datfile_sections = "terrain"

if __name__ == "__main__":

    #the convert script has 2 modes:
    #mode 0: generate structures
    #        this is completely independent from an aoe installation
    #        it generates C structures according to the gamedata formats
    #        the functionality for that is implemented in `datafile.py`
    #
    #mode 1: generate media files
    #        this requires the aoe installation
    #        database files as csv will be generated, as well as usable
    #        media files like .png and .opus.
    #        see `mediafile.py` for the implementation.

    #construct argument parser
    p = argparse.ArgumentParser(description='openage conversion script. allows usage of original media files.')

    #common options
    p.add_argument("-v", "--verbose", help="Turn on verbose log messages", action='count', default=0)
    p.add_argument("--sections", help="Define comma-separated dat file sections to export.", default=default_datfile_sections)
    #p.set_defaults(handler=lambda x: p.print_help())

    #convert script has multiple subsystems
    sp = p.add_subparsers(dest='module', help="available convert subsystems")


    #media conversion:
    media_cmd = sp.add_parser("media", help="convert media files to free formats")

    media_cmd.add_argument("-e", "--extrafiles", help = "Extract extra files that are not needed, but useful (mainly visualizations).", action='store_true')
    media_cmd.add_argument("--no-opus", help="Don't use opus conversion for audio files", action='store_true')

    mcmd_g0 = media_cmd.add_mutually_exclusive_group(required=True)
    mcmd_g0.add_argument("-o", "--output", metavar="output_directory", help="The data output directory")
    mcmd_g0.add_argument("-l", "--list-files", help="List files in the game archives", action='store_true')

    media_cmd.add_argument("srcdir", help="The Age of Empires II root directory")
    media_cmd.add_argument("extract", metavar="resource", nargs="*", help="A specific extraction rule, such as graphics:*.slp, terrain:15008.slp or *:*.wav. If no rules are specified, *:*.* is assumed")

    #set handler for media conversion
    media_cmd.set_defaults(handler=mediafile.media_convert)


    #file generations:
    genfile_cmd = sp.add_parser("structs", help="Generate C structures for handling game data")
    gcmd_g0 = genfile_cmd.add_mutually_exclusive_group(required=True)
    gcmd_g0.add_argument("-l", "--list-files", help="List all source files that can currently be generated", action='store_true')
    gcmd_g0.add_argument("-o", "--output", metavar="destination", help="Output folder for the generated files")
    genfile_cmd.add_argument("filename", nargs="*", default="*", help="Files to be generated")

    #set handler for file generation
    genfile_cmd.set_defaults(handler=datafile.data_generate)

    #actually parse argv and run main
    args = p.parse_args()

    set_verbosity(args.verbose)
    args.sections = args.sections.split(",")

    if args.module == None:
        p.print_help()
    else:
        args.handler(args)
