from PIL import Image, ImageDraw

from libc.stdint cimport int64_t
from libopenage.mapgen.perlin cimport Perlin2, Perlin3, Perlin4, value_max
from libopenage.util.vector cimport Vector2, Vector3, Vector4


def demo(argv):
    """
    Small demo for rendering 500x500 pixels of noise to noise.png
    """

    cdef Perlin2 *perlin = new Perlin2(0xc0ffeedeadbeef, 5)

    cdef int64_t scaling_factor = value_max / 128

    cdef int size_x = 10
    cdef int size_y = 10

    cdef Vector2[int64_t] point
    cdef int64_t value
    noise = Image.new('RGBA', (size_x, size_y), (255, 255, 255, 0))
    draw = ImageDraw.ImageDraw(noise)

    print("Value_max: {}".format(value_max))
    print("Scaling factor: {}".format(scaling_factor))

    for pos_x in range(size_x):
        for pos_y in range(size_y):
            point[0] = pos_x
            point[1] = pos_y
            # print("({}:{})".format(pos_x, pos_y))
            value = perlin.noise_value(point)
            # print("Value: {}".format(value))
            intensity = int(value / scaling_factor + 129)
            # print(intensity)
            draw.point((pos_x, pos_y),
                       fill=(intensity, intensity, intensity, 255))

    noise.save("noise.png")
