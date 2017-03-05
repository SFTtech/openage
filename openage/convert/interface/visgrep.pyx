# Copyright 2016-2017 the openage authors. See copying.md for legal info.

""" Python version of the visgrep utility """

import argparse
import logging
from collections import namedtuple
from PIL import Image

TOOL_DESCRIPTION = """Python translation of the visgrep v1.09
visual grep, greps for images in another image
Author of the original C version: Steve Slaven - http://hoopajoo.net"""

EPILOG = """The image.png is
scanned for detect.png starting from X,Y specified above. When detect.png
is found, then all the match.png files are scanned at an offset of x,y as
specified above.  If a match is found, then visgrep prints the x,y and
index of the item.

For example, image.png is a screenshot and match1.png .. to match5.png are
images of letters a to e.  Each of these letters is enclosed in a blue box,
so detect.png is an image of the upper left corner of the box.  This box is
not included in the match*.png files, so they are actually offset 5 pixels
down and 4 pixels to the left.  You might run it like this then:

  visgrep -b -t50 -x-4 -y5 image.png match_corner.png match_a.png match_b.png ...

Etc, with all matches listed.  Now suppose the screen showed 'ace' so
visgrep might output:

0 10,10 0
12 50,10 2
7 90,10 4

Showing that match_a.png (index 0) is at 10,10 on the screen.  If no match
is found even though the detection image is found, the index will be -1.

The first match was 100%% accurate, while the second and third were very slightly
inaccurate, probably due to anti-aliasing on the fonts.

Exit status is 0 for successful match, 1 for no match, and 2 for error.

See the examples page for use cases for different flags"""

# pylint: disable=no-member
Point = namedtuple('Point', ['x', 'y'])
Size = namedtuple('Size', ['width', 'height'])
Pixel = namedtuple('Pixel', ['r', 'g', 'b', 'a'])
FoundResult = namedtuple('FoundResult', ['badness', 'point'])


def img_pixel_get(img, point):
    """
    Get pixel color or zero if outside bounds.
    """
    if point.x < img.size[0] and point.y < img.size[1]:
        return Pixel(*img.getpixel(point))

    return Pixel(0, 0, 0, 0)


def img_subimage_find(master, find, start_from, tolerance, find_next):
    """
    Fuzzily find a part of an image that matches the patern.
    """
    x_end = master.size[0] - find.size[0]
    y_end = master.size[1] - find.size[1]

    if find_next:
        start_from = Point(start_from.x + 1, start_from.y)

    logging.debug("Starting from %d", start_from)
    logging.debug("End %d,%d", x_end, y_end)

    # Loop the whole freakin image looking for this sub image, but not past edges
    for y_it in range(start_from.y, y_end + 1):
        logging.debug("Begin subimg find loop for y %d of %d", y_it, y_end)
        for x_it in range(start_from.x, x_end + 1):
            point = Point(x_it, y_it)
            logging.debug("Begin subimg find loop for x: %d,%d", point.x, point.y)
            badness = img_subimage_cmp(master, find, point, tolerance)
            if badness <= tolerance:
                logging.debug("Found subimage at %d,%d", point.x, point.y)
                return FoundResult(badness, point)

    # No match
    return FoundResult(-1, Point(-1, -1))


def img_pixel_cmp(pix, other_pix):
    """
    o is the compare from pixel, assumed to be from a pattern. It's transparency
    is the transparency used to modify the tolerance value
      return( memcmp( &p, &o, sizeof( PIXEL ) ) );
    make tolerance mean something
    """
    difference = abs(pix.r - other_pix.r) + abs(pix.g - other_pix.g) + abs(pix.b - other_pix.b)
    transparentness = other_pix.a
    difference = difference * (transparentness / 255)
    logging.debug("Difference: %d", difference)
    return difference


def img_subimage_cmp(master, subimage, where, tolerance):
    """
    Returns 0 if subimage is inside master at where, like *cmp usually does for other stuff
    otherwise returns an integer of how different the match is, for each color component
    value off.  tolerance is how high to go before bailing.  set lower to avoid processing
    lots of extra pixels, it will just ret when tolerance is met
    """
    mpt = Point(0, 0)

    logging.debug("Comparing subimage where=%d,%d", where.x, where.y)

    # Check if subimage even fits in masterimage at POINT
    if ((where.x + subimage.size[0]) > master.size[0] or
            (where.y + subimage.size[1]) > master.size[1]):
        # Superbad
        logging.debug("Subimage would not fit here")
        return 1000

    badness = 0
    for sptx in range(subimage.size[0]):
        for spty in range(subimage.size[1]):
            # Map U/V to X/Y
            mpt = Point(sptx + where.x, spty + where.y)

            logging.debug("Pixel cmp @ main: %d,%d, subimage: %d,%d", mpt.x, mpt.y, sptx, spty)

            # Grab pels and see if they match
            mpx = img_pixel_get(master, mpt)
            spx = img_pixel_get(subimage, Point(sptx, spty))

            badness += abs(img_pixel_cmp(mpx, spx))

            if badness > tolerance:
                logging.debug("Bail out early, badness > tolerance %d > %d", badness, tolerance)
                # No match here, bail early
                return badness

    # Matched all of subimage
    logging.debug("Image match ok, badness = %d", badness)
    return badness


def do_output(show_badness, badness, point, idx):
    """
    Print match coordinates and score.
    """
    if show_badness:
        print("%d %d,%d %d" % (badness, point.x, point.y, idx))
    else:
        print("%d,%d %d" % (point.x, point.y, idx))


def advance_point(point, img, start_x):
    """
    Move across the image.
    """
    next_point = Point(point.x + 1, point.y)

    if next_point.x > img.size[0]:
        next_point = Point(start_x, next_point.y + 1)
        if next_point.y > img.size[1]:
            # done, bail
            next_point = Point(-1, -1)

    return next_point


def img_match_any(img, patterns, off, tolerance, pt_match):
    """
    Move across the image.
    """
    gotmatch = None

    tmp_pt_x = pt_match.point.x + off.x
    tmp_pt_y = pt_match.point.y + off.y

    for cnt, pattern in enumerate(patterns):
        if gotmatch is None:
            logging.info(" Testing for %d  ", cnt)
            logging.info(" %d,%d ", tmp_pt_x, tmp_pt_y)
            badness = img_subimage_cmp(img,
                                       pattern,
                                       Point(tmp_pt_x, tmp_pt_y),
                                       tolerance)

            if badness <= tolerance:
                logging.info("  YES")

                gotmatch = (FoundResult(badness, Point(tmp_pt_x, tmp_pt_y)), cnt)

                # Fall out
                break
            else:
                logging.info("  NO")

    return gotmatch


GeomParams = namedtuple('GeomParams', ['off', 'src', 'sub', 'start', 'pattern_off', 'pattern'])
GeomParams.__new__.__defaults__ = (Point(0, 0),
                                   Size(None, None),
                                   Size(1, 1),
                                   Point(0, 0),
                                   None,
                                   None)

MetricParams = namedtuple('MetricParams', ['tolerance'])
ImgParams = namedtuple('ImgParams', ['img', 'detect', 'match', 'scan_all'])


def visgrep(image, pattern, tolerance, height=None):
    """
    Return points where pattern is found in the image.
    """
    geom_params = GeomParams(src=Size(None, height))
    metric_params = MetricParams(tolerance)

    pattern_rgba = pattern.convert('RGBA')
    img_params = ImgParams(image.convert('RGBA'), pattern_rgba, [pattern_rgba], False)

    return [r[0] for r in visgrep_cli(geom_params, metric_params, img_params)]


def visgrep_cli(geom_params, metric_params, img_params):
    """
    Perform search.
    """
    pt_match = FoundResult(0, Point(0, 0))
    results = []
    find_next = False

    img = img_params.img.crop((0, 0,
                               geom_params.src.width or img_params.img.size[0],
                               geom_params.src.height or img_params.img.size[1]))

    if geom_params.pattern_off is not None:
        patterns_crop = (geom_params.pattern_off.x,
                         geom_params.pattern_off.y,
                         geom_params.pattern.width + geom_params.pattern_off.x,
                         geom_params.pattern.height + geom_params.pattern_off.y)

        find = img_params.detect.crop(patterns_crop)
        matches = [im.crop(patterns_crop) for im in img_params.match]
    else:
        find = img_params.detect
        matches = img_params.match

    logging.info("Detecting offsets...")
    pt_match = FoundResult(pt_match.badness, Point(geom_params.start.x, geom_params.start.y))
    find_next = False
    while pt_match.point.x != -1:
        if img_params.scan_all:
            # fake match here
            if find_next:
                # increment counters
                pt_match = FoundResult(pt_match.badness,
                                       advance_point(pt_match.point, img, geom_params.start.x))
        else:
            pt_match = img_subimage_find(img,
                                         find,
                                         pt_match.point,
                                         metric_params.tolerance,
                                         find_next)

        # Not first time anymore
        find_next = True

        if pt_match.point.x != -1:
            logging.info("  Found match at %d,%d", pt_match.point.x, pt_match.point.y)

            if len(img_params.match) == 1 and img_params.match[0] == img_params.detect:
                # Detection pettern is the single match pattern
                results.append((pt_match, 0))
                continue

            # Try and identify what thing it is
            gotmatch = None
            import itertools
            for tmp_off_x, tmp_off_y in itertools.product(range(geom_params.sub.width),
                                                          range(geom_params.sub.height)):
                gotmatch = img_match_any(img,
                                         matches,
                                         Point(geom_params.off.x + tmp_off_x,
                                               geom_params.off.y + tmp_off_y),
                                         metric_params.tolerance,
                                         pt_match)

                if gotmatch is not None:
                    results.append(gotmatch)
                    break

            # Notify of no match
            if gotmatch is None:
                logging.info(" NO ITEMS MATCHED!")
                if not img_params.scan_all:
                    results.append((pt_match, -1))

    return results


def main():
    """
    Visual grep, greps for images in another image.
    """
    parser = argparse.ArgumentParser(description=TOOL_DESCRIPTION, epilog=EPILOG,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument("-x", dest="off_x", default=0, metavar="X_OFF", type=int,
                        help="Set x offset for detection matching")
    parser.add_argument("-y", dest="off_y", default=0, metavar="Y_OFF", type=int,
                        help="Set y offset for detection matching")
    parser.add_argument("-I", dest="src_width", default=0, metavar="WIDTH", type=int,
                        help="Set width for cropping the source image")
    parser.add_argument("-J", dest="src_height", default=0, metavar="HEIGHT", type=int,
                        help="Set height for cropping the source image")
    parser.add_argument("-W", dest="sub_width", default=0, metavar="X_OFF_WIDTH", type=int,
                        help="Set x offset width for detection matching")
    parser.add_argument("-H", dest="sub_height", default=0, metavar="Y_OFF_HEIGHT", type=int,
                        help="Set y offset height for detection matching")
    parser.add_argument("-X", dest="start_x", default=0, metavar="START_X_OFF", type=int,
                        help="Start scanning at X")
    parser.add_argument("-Y", dest="start_y", default=0, metavar="START_Y_OFF", type=int,
                        help="Start scanning at Y")
    parser.add_argument("-u", dest="off_u", default=0, metavar="U_OFF", type=int,
                        help="Set u offset for cropping patterns before use")
    parser.add_argument("-v", dest="off_v", default=0, metavar="V_OFF", type=int,
                        help="Set y offset for cropping patterns before use")
    parser.add_argument("-M", dest="pattern_width", metavar="U_WIDTH", type=int,
                        help="Set width of the cropped patterns")
    parser.add_argument("-N", dest="pattern_height", metavar="V_HEIGHT", type=int,
                        help="Set height of the cropped patterns")
    parser.add_argument("-a", dest="scan_all", help="""Scan all patterns, not just after matching
                        the detection pattern note: this method is much slower because we scan for
                        all images at every pixel instead of just at detection points. Also, in
                        this mode the detection image is ignored, there will be no matches
                        for tile -1""", action="store_true")
    parser.add_argument("-t", dest="tolerance", default=0, metavar="TOLERANCE", type=int,
                        help="Set tolerance for 'fuzzy' matches, higher numbers are more tolerant")
    parser.add_argument("-b", dest="show_badness",
                        help="""Display 'badness' value, higher numbers mean match is less accurate,
                        a badness value of 0 means the match is pixel-perfect""",
                        action="store_true")
    parser.add_argument("-d", metavar="DEBUGLEVEL", type=int, help="Print debug messages")
    parser.add_argument("image", metavar='image.png', help="Image to search in")
    parser.add_argument("detect", metavar='detect.png', help="Pattern to search")
    parser.add_argument("match", metavar='match.png', nargs="*",
                        help="Images to compare if detected")

    args = parser.parse_args()

    logging.basicConfig(level=args.d)

    pattern_crop_params = [v is not None for v in
                           (args.off_u, args.off_v, args.pattern_width, args.pattern_height)]

    if any(pattern_crop_params) and not all(pattern_crop_params):
        parser.error('-u, -v, -M, -N must be given together')

    geom_params = GeomParams(Point(args.off_x, args.off_y),
                             Size(args.src_width, args.src_height),
                             Size(args.sub_width, args.sub_height),
                             Point(args.start_x, args.start_y),
                             Point(args.off_u, args.off_v),
                             Size(args.pattern_width, args.pattern_height))

    metric_params = MetricParams(args.tolerance)

    image = Image.open(args.image).convert('RGBA')
    find = Image.open(args.detect).convert('RGBA')
    match = [Image.open(fname).convert('RGBA') for fname in args.match]

    img_params = ImgParams(image, find, match, args.scan_all)

    results = visgrep_cli(geom_params, metric_params, img_params)

    exit_status = 1

    for result in results:
        do_output(args.show_badness,
                  result[0].badness,
                  result[0].point,
                  result[1])

        if result[1] != -1:
            exit_status = 0

    exit(exit_status)


if __name__ == "__main__":
    main()
