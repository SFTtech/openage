# Copyright 2018-2022 the openage authors. See copying.md for legal info.
"""
Demo for the opusenc module.
"""

import argparse
import time

from . import opusenc
from .....log import info, crit


def convert(args) -> int:
    """ Demonstrates the usage of the opusenc module. """

    cli = argparse.ArgumentParser()
    cli.add_argument("input", metavar='file.wav',
                     help="a wave file that should be converted")
    cli.add_argument("--out", "-o", default=None, metavar='file.opus',
                     help="the name for the resulting opus file")
    args = cli.parse_args(args)

    wavname = args.input
    info("Reading %s...", wavname)

    with open(wavname, mode='rb') as wav_file:
        wav = wav_file.read()

    info("Encoding...")

    tic = time.time()
    out = opusenc.encode(wav)
    tic = time.time() - tic

    if isinstance(out, (str, int)):
        crit("Encoding failed: %s", out)
        return 1

    outname = args.out
    if outname is None:
        outname = wavname[:-3] + "opus" if wavname[-3:] == "wav" else wavname + ".opus"

    info("Writing %s.", outname)
    with open(outname, mode='wb') as fil:
        fil.write(out)

    info("Wave size: %s", f"{len(wav):>8}")
    info("Opus size: %s (%s%)", f"{len(out):>8}",
         f"{100 * len(out) / len(wav):2.1f}")
    info("Encoding time: %s seconds.", f"{tic:.3f}")
    return 0
