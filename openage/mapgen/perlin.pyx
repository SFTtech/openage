from PIL import Image, ImageDraw

from libc.stdint cimport int64_t
from libopenage.mapgen.perlin cimport Perlin2, Perlin3, Perlin4, value_max
from libopenage.util.vector cimport Vector2, Vector3, Vector4


def demo():
    """
    Small demo for rendering 500x500 pixels of noise to noise.png
    """

    cdef Perlin2 perlin = Perlin2(0xdeadbeef, 25)

    cdef int scaling_factor = value_max / 256

    cdef int size_x = 500
    cdef int size_y = 500

    cdef Vector2 point
    cdef int64_t value
    noise = Image.new('RGBA', (size_x, size_y), (255, 255, 255, 0))
    draw = ImageDraw.ImageDraw(noise)

    for pos_x in range(size_x):
        for pos_y in range(size_y):
            point = Vector2(size_x, size_y)
            value = perlin.noise_value(point)
            intensity = int(value / scaling_factor)
            draw.point((pos_x, pos_y),
                       fill=(intensity, intensity, intensity, 255))

    noise.save("noise.png")
