# Copyright 2018-2018 the openage authors. See copying.md for legal info.
"""
Demo for the opusenc module.
"""

import argparse
import time

from . import opusenc
from ...log import info, crit


def convert(args):
    """ Demonstrates the usage of the opusenc module. """

    cli = argparse.ArgumentParser()
    cli.add_argument("input", metavar='file.wav',
                     help="a wave file that should be converted")
    cli.add_argument("--out", "-o", default=None, metavar='file.opus',
                     help="the name for the resulting opus file")
    args = cli.parse_args(args)

    wavname = args.input
    info("Reading {}...".format(wavname))
    wav = open(wavname, mode='rb').read()

    info("Encoding...")

    tic = time.time()
    out = opusenc.encode(wav)
    tic = time.time() - tic

    if isinstance(out, (str, int)):
        crit("Encoding failed: {}".format(out))
        return 1

    outname = args.out
    if outname is None:
        outname = wavname[:-3] + "opus" if wavname[-3:] == "wav" else wavname + ".opus"

    info("Writing {}.".format(outname))
    with open(outname, mode='wb') as fil:
        fil.write(out)

    info("Wave size: {:>8}".format(len(wav)))
    info("Opus size: {:>8} ({:2.1f}%)".format(len(out), 100 * len(out) / len(wav)))
    info("Encoding time: {:.3f} seconds.".format(tic))
    return 0
