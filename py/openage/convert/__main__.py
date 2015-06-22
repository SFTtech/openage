# Copyright 2013-2015 the openage authors. See copying.md for legal info.

import argparse
from openage.log import set_verbosity

from . import datafile
from . import mediafile


def main():
    # the convert script has 1 mode:
    # mode 0: generate media files
    #         this requires the aoe installation
    #         database files as csv will be generated, as well as usable
    #         media files like .png and .opus.
    #         see `mediafile.py` for the implementation.

    # construct argument parser
    p = argparse.ArgumentParser(
        description="openage conversion script."
                    "converts original media files to usable formats.")

    # common options
    p.add_argument("-v", "--verbose", action='count', default=0,
                   help="Turn on verbose log messages")

    # convert script has multiple subsystems
    sp = p.add_subparsers(dest='module',
                          help="available convert subsystems")

    # media conversion:
    media_cmd = sp.add_parser("media",
                              help="convert media files to free formats")

    media_cmd.add_argument("-e", "--extrafiles", action='store_true',
                           help="Extract extra files that are not needed, "
                                "but useful (mainly visualizations).")
    media_cmd.add_argument("--no-opus", action='store_true',
                           help="Don't use opus conversion for audio files")
    media_cmd.add_argument("--use-dat-cache", action='store_true',
                           help="Use cache file for the empires.dat file")
    media_cmd.add_argument("-j", "--jobs", type=int, default=0,
                           help="How many jobs to use in parallel")

    mcmd_g0 = media_cmd.add_mutually_exclusive_group(required=True)
    mcmd_g0.add_argument("-o", "--output", metavar="output_directory",
                         help="The data output directory")
    mcmd_g0.add_argument("-l", "--list-files", action='store_true',
                         help="List files in the game archives")

    media_cmd.add_argument("srcdir",
                           help="The Age of Empires II root directory")
    media_cmd.add_argument("extract", metavar="resource", nargs="*",
                           help="A specific extraction rule, "
                                "such as graphics:*.slp, terrain:15008.slp "
                                "or *:*.wav. If no rules are specified, "
                                "*:*.* is assumed")

    # set handler for media conversion
    media_cmd.set_defaults(handler=mediafile.media_convert)

    # actually parse argv and run main
    args = p.parse_args()

    set_verbosity(args.verbose)

    if args.module is None:
        p.print_help()
    else:
        args.handler(args)

if __name__ == "__main__":
    main()
