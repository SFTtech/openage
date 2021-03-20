# Copyright 2021-2021 the openage authors. See copying.md for legal info.

cdef class Packer:
    cdef unsigned int margin
    cdef dict mapping

    cdef void pack(self, blocks)
    cdef (unsigned int, unsigned int) pos(self, block)
    cdef unsigned int width(self)
    cdef unsigned int height(self)

cdef class DeterministicPacker(Packer):
    pass

cdef class BestPacker:
    cdef list packers
    cdef Packer current_best

    cdef void pack(self, blocks)
    cdef Packer best_packer(self)
    cdef (unsigned int, unsigned int) pos(self, block)
    cdef unsigned int width(self)
    cdef unsigned int height(self)

cdef class RowPacker(Packer):
    pass

cdef class ColumnPacker(Packer):
    pass

cdef class BinaryTreePacker(Packer):
    cdef unsigned int aspect_ratio
    cdef PackerNode root

    cdef void fit(self, block)
    cdef PackerNode find_node(self, PackerNode root, unsigned int width, unsigned int height)
    cdef PackerNode split_node(self, PackerNode node, unsigned int width, unsigned int height)
    cdef PackerNode grow_node(self, unsigned int width, unsigned int height)
    cdef PackerNode grow_right(self, unsigned int width, unsigned int height)
    cdef PackerNode grow_down(self, unsigned int width, unsigned int height)

cdef class PackerNode:
    cdef public unsigned int x, y, width, height
    cdef public bint used
    cdef public PackerNode down, right
