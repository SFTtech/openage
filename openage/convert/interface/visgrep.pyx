# Copyright 2016-2017 the openage authors. See copying.md for legal info.

# If you wanna boost speed even further:
# cython: profile=False


""" Cython version of the visgrep utility """

import argparse
cimport cython
import itertools
import logging
import numpy
cimport numpy

from collections import namedtuple
from PIL import Image
from sys import exit


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


ctypedef numpy.uint8_t pixel_t

cdef struct pixel:
    pixel_t r
    pixel_t g
    pixel_t b
    pixel_t a


Point = namedtuple('Point', ['x', 'y'])
Size = namedtuple('Size', ['width', 'height'])
FoundResult = namedtuple('FoundResult', ['badness', 'point'])

GeomParams = namedtuple('GeomParams', ['off', 'src', 'sub', 'start', 'pattern_off', 'pattern'])
GeomParams.__new__.__defaults__ = (Point(0, 0),
                                   Size(None, None),
                                   Size(1, 1),
                                   Point(0, 0),
                                   None,
                                   None)

MetricParams = namedtuple('MetricParams', ['tolerance'])
ImgParams = namedtuple('ImgParams', ['img', 'detect', 'match', 'scan_all'])


cdef numpy.ndarray img_to_array(img):
    """
    Convert a PIL image to a numpy array.
    """

    if not isinstance(img, Image.Image):
        raise ValueError("PIL image required, not '%s'" % type(img))

    return numpy.array(img)


cpdef numpy.ndarray crop_array(array, corners):
    """
    crop a numpy array by the absolute corner coordinates:

    (x0, y0, x1, y1) = (left, upper, right, lower)

    Those corners are all INCLUSIVE.

    Array must have shape (h, w, 4), i.e. an RGBA image.
    """

    x0, y0, x1, y1 = corners
    return array[y0:y1, x0:x1]


@cython.boundscheck(False)
@cython.wraparound(False)
cdef inline pixel img_pixel_get(numpy.ndarray[pixel_t, ndim=3] img,
                                Py_ssize_t x, Py_ssize_t y):
    """
    Get pixel color or zero if outside bounds.
    Totally speed boosted.
    """

    if x < img.shape[1] and y < img.shape[0]:
        return pixel(img[y, x, 0],
                     img[y, x, 1],
                     img[y, x, 2],
                     img[y, x, 3])

    return pixel(0, 0, 0, 0)


@cython.boundscheck(False)
@cython.wraparound(False)
cdef img_subimage_find(numpy.ndarray[pixel_t, ndim=3] master,
                       numpy.ndarray[pixel_t, ndim=3] find,
                       start_from, float tolerance, find_next):
    """
    Fuzzily find a part of an image that matches the pattern.
    """

    cdef Py_ssize_t x_end = master.shape[1] - find.shape[1]
    cdef Py_ssize_t y_end = master.shape[0] - find.shape[0]

    if find_next:
        start_from = Point(start_from.x + 1, start_from.y)

    cdef Py_ssize_t ymax = start_from.y
    cdef Py_ssize_t xmax = start_from.x
    cdef Py_ssize_t y_it
    cdef Py_ssize_t x_it
    cdef float badness
    # Loop the whole freakin image looking for this sub image, but not past edges
    for y_it in range(ymax, y_end + 1):
        for x_it in range(xmax, x_end + 1):
            badness = img_subimage_cmp(master, find, x_it, y_it, tolerance)
            if badness <= tolerance:
                return FoundResult(badness, Point(x_it, y_it))

    # No match
    return FoundResult(-1, Point(-1, -1))


#@cython.profile(False)
cdef inline float img_pixel_cmp(const pixel &pix, const pixel &other_pix):
    """
    o is the compare from pixel, assumed to be from a pattern. It's transparency
    is the transparency used to modify the tolerance value
      return( memcmp( &p, &o, sizeof( PIXEL ) ) );
    make tolerance mean something
    """
    cdef int difference = abs(pix.r - other_pix.r) + abs(pix.g - other_pix.g) + abs(pix.b - other_pix.b)
    cdef int transparentness = other_pix.a
    return difference * (transparentness / 255)


@cython.boundscheck(False)
@cython.wraparound(False)
@cython.nonecheck(False)
cdef float img_subimage_cmp(numpy.ndarray[pixel_t, ndim=3] master,
                            numpy.ndarray[pixel_t, ndim=3] subimage,
                            Py_ssize_t where_x,
                            Py_ssize_t where_y,
                            float tolerance):
    """
    Returns 0 if subimage is inside master at where, like *cmp usually does for other stuff
    otherwise returns an integer of how different the match is, for each color component
    value off.  tolerance is how high to go before bailing.  set lower to avoid processing
    lots of extra pixels, it will just ret when tolerance is met
    """

    logging.debug("Comparing subimage where=%d,%d", where_x, where_y)

    # Check if subimage even fits in masterimage at POINT
    if ((where_x + subimage.shape[1]) > master.shape[1] or
        (where_y + subimage.shape[0]) > master.shape[0]):
        # Superbad
        logging.debug("Subimage would not fit here")
        return 1000

    cdef float badness = 0

    cdef Py_ssize_t sptx
    cdef Py_ssize_t spty

    for sptx in range(subimage.shape[1]):
        for spty in range(subimage.shape[0]):
            # Map U/V to X/Y.
            # Grab pels and see if they match
            mpx = img_pixel_get(master, sptx + where_x, spty + where_y)
            spx = img_pixel_get(subimage, sptx, spty)

            badness += abs(img_pixel_cmp(mpx, spx))

            if badness > tolerance:
                logging.debug("Bail out early, badness > tolerance %d > %d", badness, tolerance)
                # No match here, bail early
                return badness

    # Matched all of subimage
    logging.debug("Image match ok, badness = %d", badness)
    return badness


cdef do_output(show_badness, badness, point, idx):
    """
    Print match coordinates and score.
    """
    if show_badness:
        print("%d %d,%d %d" % (badness, point.x, point.y, idx))
    else:
        print("%d,%d %d" % (point.x, point.y, idx))


cdef advance_point(point, img, start_x):
    """
    Move across the image.
    """
    next_point = point(point.x + 1, point.y)

    if next_point.x > img.shape[1]:
        next_point = point(start_x, next_point.y + 1)
        if next_point.y > img.shape[0]:
            # done, bail
            next_point = point(-1, -1)

    return next_point


cdef img_match_any(img, patterns, off, tolerance, pt_match):
    """
    Move across the image.
    """
    gotmatch = None

    tmp_pt_x = pt_match.point.x + off.x
    tmp_pt_y = pt_match.point.y + off.y

    for cnt, pattern in enumerate(patterns):
        if gotmatch is None:
            logging.info(" Testing for pattern %d", cnt)
            logging.info(" %d,%d ", tmp_pt_x, tmp_pt_y)
            badness = img_subimage_cmp(img,
                                       pattern,
                                       tmp_pt_x,
                                       tmp_pt_y,
                                       tolerance)

            if badness <= tolerance:
                logging.info("  YES")

                gotmatch = (FoundResult(badness, Point(tmp_pt_x, tmp_pt_y)), cnt)

                # Fall out
                break
            else:
                logging.info("  NO")

    return gotmatch


cpdef visgrep(image, pattern, tolerance, height=None):
    """
    Return points where pattern is found in the image.

    image and pattern are numpy arrays of RGBA images.
    """

    if not (isinstance(image, numpy.ndarray) and
            isinstance(pattern, numpy.ndarray)):
        raise ValueError("image and pattern must be a numpy array")

    geom_params = GeomParams(src=Size(None, height))
    metric_params = MetricParams(tolerance)

    img_params = ImgParams(image, pattern, [pattern], False)

    ret = list()
    for find in visgrep_cli(geom_params, metric_params, img_params):
        ret.append(find[0])

    return ret


cdef visgrep_cli(geom_params, metric_params, img_params):
    """
    Perform search, return list of results.
    """

    pt_match = FoundResult(0, Point(0, 0))
    results = []
    find_next = False

    # we'll search in this image.
    img = crop_array(img_params.img,
                     (0, 0,
                      geom_params.src.width or img_params.img.shape[1],
                      geom_params.src.height or img_params.img.shape[0]))

    if geom_params.pattern_off is not None:
        patterns_crop = (geom_params.pattern_off.x,
                         geom_params.pattern_off.y,
                         geom_params.pattern.width + geom_params.pattern_off.x,
                         geom_params.pattern.height + geom_params.pattern_off.y)

        find = crop_array(img_params.detect, patterns_crop)

        matches = [crop_array(match, patterns_crop)
                   for match in img_params.match]
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
                next_point = advance_point(pt_match.point, img, geom_params.start.x)

                # increment counters
                pt_match = FoundResult(pt_match.badness, next_point)
        else:
            pt_match = img_subimage_find(img, find, pt_match.point,
                                         metric_params.tolerance,
                                         find_next)

        # Not first time anymore
        find_next = True

        if pt_match.point.x != -1:
            logging.info("  Found match at %d,%d", pt_match.point.x, pt_match.point.y)

            if len(img_params.match) == 1 and\
               numpy.array_equal(img_params.match[0], img_params.detect):
                # Detection pattern is the single match pattern
                results.append((pt_match, 0))
                continue

            # Try and identify what thing it is
            gotmatch = None
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

    image = img_to_array(Image.open(args.image).convert('RGBA'))
    find = img_to_array(Image.open(args.detect).convert('RGBA'))
    match = [img_to_array(Image.open(fname).convert('RGBA'))
             for fname in args.match]

    geom_params = GeomParams(Point(args.off_x, args.off_y),
                             Size(args.src_width, args.src_height),
                             Size(args.sub_width, args.sub_height),
                             Point(args.start_x, args.start_y),
                             Point(args.off_u, args.off_v),
                             Size(args.pattern_width, args.pattern_height))
    metric_params = MetricParams(args.tolerance)
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
