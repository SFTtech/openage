# Copyright 2021-2024 the openage authors. See copying.md for legal info.

from libcpp.unordered_map cimport unordered_map

ctypedef (unsigned int, unsigned int, (unsigned int, unsigned int)) mapping_value

cdef class Packer:
    cdef unsigned int margin
    cdef unordered_map[int, mapping_value] mapping

    cdef void pack(self, list blocks)
    cdef (unsigned int, unsigned int) pos(self, int index)
    cdef unsigned int width(self)
    cdef unsigned int height(self)
    cdef list get_mapping_hints(self, list blocks)
    cdef (unsigned int) get_packer_settings(self)

cdef class DeterministicPacker(Packer):
    pass

cdef class BestPacker:
    cdef list packers
    cdef Packer current_best

    cdef void pack(self, list blocks)
    cdef Packer best_packer(self)
    cdef (unsigned int, unsigned int) pos(self, int index)
    cdef unsigned int width(self)
    cdef unsigned int height(self)
    cdef list get_mapping_hints(self, list blocks)
    cdef (unsigned int) get_packer_settings(self)

cdef class RowPacker(Packer):
    pass

cdef class ColumnPacker(Packer):
    pass

cdef class BinaryTreePacker(Packer):
    cdef unsigned int aspect_ratio
    cdef packer_node *root

    cdef void fit(self, block block)
    cdef (unsigned int) get_packer_settings(self)
    cdef packer_node *find_node(self, packer_node *root, unsigned int width, unsigned int height) noexcept
    cdef packer_node *split_node(self, packer_node *node, unsigned int width, unsigned int height) noexcept
    cdef packer_node *grow_node(self, unsigned int width, unsigned int height) noexcept
    cdef packer_node *grow_right(self, unsigned int width, unsigned int height) noexcept
    cdef packer_node *grow_down(self, unsigned int width, unsigned int height) noexcept

cdef struct packer_node:
    unsigned int x
    unsigned int y
    unsigned int width
    unsigned int height
    bint used
    packer_node *down
    packer_node *right

cdef struct block:
    unsigned int index
    unsigned int width
    unsigned int height
