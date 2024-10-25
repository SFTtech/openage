# Copyright 2016-2024 the openage authors. See copying.md for legal info.
#
# cython: infer_types=True,profile=False
# TODO pylint: disable=C,R

"""
Routines for 2D binpacking
"""

cimport cython
from libc.stdint cimport uintptr_t
from libc.stdlib cimport malloc

from libc.math cimport sqrt


@cython.boundscheck(False)
@cython.wraparound(False)
@cython.cdivision(True)
cdef inline (unsigned int, unsigned int) factor(unsigned int n):
    """
    Return two (preferable close) factors of n.
    """
    cdef unsigned int a = <unsigned int>sqrt(n)
    cdef int num
    for num in range(a, 0, -1):
        if n % num == 0:
            return num, n // num


cdef class Packer:
    """
    Packs blocks.
    """
    def __init__(self, int margin):
        self.margin = margin
        self.mapping = {}

    cdef void pack(self, list blocks):
        """
        Pack all the blocks.

        Each block must have a width and height attribute.
        """
        raise NotImplementedError

    cdef (unsigned int, unsigned int) pos(self, int index):
        return self.mapping[index]

    cdef unsigned int width(self):
        """
        Gets the total width of the packing.
        """
        return max(self.pos(i)[0] + block[2][0] for i, block in self.mapping.items())

    cdef unsigned int height(self):
        """
        Gets the total height of the packing.
        """
        return max(self.pos(i)[1] + block[2][1] for i, block in self.mapping.items())

    cdef tuple get_packer_settings(self):
        """
        Get the init parameters set for the packer.
        """
        return (self.margin,)

    cdef list get_mapping_hints(self, list blocks):
        cdef list hints = []
        for block in blocks:
            hints.append(self.pos(block[2]))

        return hints


cdef class DeterministicPacker(Packer):
    """
    Packs blocks based on predetermined settings.
    """

    def __init__(self, int margin, list hints):
        super().__init__(margin)
        self.hints = hints

    cdef void pack(self, list blocks):
        for idx, block in enumerate(blocks):
            self.mapping[block[2]] = self.hints[idx]


cdef class BestPacker:
    """
    Chooses the best result from all the given packers.
    """
    def __init__(self, list packers):
        self.packers = packers
        self.current_best = None

    cdef void pack(self, list blocks):
        cdef Packer packer
        for packer in self.packers:
            packer.pack(blocks)

        self.current_best = self.best_packer()

    cdef Packer best_packer(self):
        return min(self.packers, key=lambda Packer p: p.width() * p.height())

    cdef (unsigned int, unsigned int) pos(self, int index):
        return self.current_best.pos(index)

    cdef unsigned int width(self):
        return self.current_best.width()

    cdef unsigned int height(self):
        return self.current_best.height()

    cdef tuple get_packer_settings(self):
        return self.current_best.get_packer_settings()

    cdef list get_mapping_hints(self, list blocks):
        return self.current_best.get_mapping_hints(blocks)


cdef class RowPacker(Packer):
    """
    Packs blocks into rows, greedily trying to minimize the maximum width.
    """

    cdef void pack(self, list blocks):
        self.mapping = {}

        cdef unsigned int num_rows
        cdef list rows

        num_rows, _ = factor(len(blocks))
        rows = [[] for _ in range(num_rows)]

        # Put blocks into rows.
        for block in blocks:
            min_row = min(rows, key=lambda row: sum(b[0] for b in row))
            min_row.append(block)

        # Calculate positions.
        y = 0
        for row in rows:
            x = 0

            for block in row:
                self.mapping[block[2]] = (x, y)
                x += block[0] + self.margin

            y += max(block[1] for block in row) + self.margin


cdef class ColumnPacker(Packer):
    """
    Packs blocks into columns, greedily trying to minimize the maximum height.
    """

    cdef void pack(self, list blocks):
        self.mapping = {}

        num_columns, _ = factor(len(blocks))
        columns = [[] for _ in range(num_columns)]

        # Put blocks into columns.
        for block in blocks:
            min_col = min(columns, key=lambda col: sum(b[1] for b in col))
            min_col.append(block)

        # Calculate positions.
        x = 0
        for column in columns:
            y = 0

            for block in column:
                self.mapping[block[2]] = (x, y)
                y += block[1] + self.margin

            x += max(block[0] for block in column) + self.margin


cdef inline (unsigned int, unsigned int, unsigned int, unsigned int) maxside_heuristic(block):
    """
    Heuristic: Order blocks by maximum side.
    """
    return (max(block[0], block[1]),
            min(block[0], block[1]),
            block[1],
            block[0])


cdef class BinaryTreePacker(Packer):
    """
    Binary tree bin packing strategy.

    Very close to http://codeincomplete.com/posts/2011/5/7/bin_packing/.

    Aditionally can target a given aspect ratio. 97/49 is optimal for terrain
    textures.
    """

    def __init__(self, int margin, int aspect_ratio=1):
        # ASF: what about heuristic=max_heuristic?
        super().__init__(margin)
        self.aspect_ratio = aspect_ratio
        self.root = NULL

    cdef void pack(self, list blocks):
        self.mapping = {}
        self.root = NULL

        for block in sorted(blocks, key=maxside_heuristic, reverse=True):
            self.fit(block)

    cdef (unsigned int, unsigned int) pos(self, int index):
        node = self.mapping[index]
        return node[0], node[1]

    cdef tuple get_packer_settings(self):
        return (self.margin,)

    cdef void fit(self, block):
        cdef packer_node *node
        if self.root == NULL:
            self.root = <packer_node *>malloc(sizeof(packer_node))
            self.root.x = 0
            self.root.y = 0
            self.root.width = block[0] + self.margin
            self.root.height = block[1] + self.margin
            self.root.used = False
            self.root.down = NULL
            self.root.right = NULL

        node = self.find_node(self.root,
                              block[0] + self.margin,
                              block[1] + self.margin)

        if node != NULL:
            node = self.split_node(node,
                                   block[0] + self.margin,
                                   block[1] + self.margin)
        else:
            node = self.grow_node(block[0] + self.margin,
                                  block[1] + self.margin)

        self.mapping[block[2]] = (node.x, node.y, (block[0], block[1]))

    cdef packer_node *find_node(self, packer_node *root, unsigned int width, unsigned int height):
        if root.used:
            return (self.find_node(root.right, width, height) or
                    self.find_node(root.down, width, height))

        elif width <= root.width and height <= root.height:
            return root

    cdef packer_node *split_node(self, packer_node *node, unsigned int width, unsigned int height):
        node.used = True

        node.down = <packer_node *>malloc(sizeof(packer_node))
        node.down.x = node.x
        node.down.y = node.y + height
        node.down.width = node.width
        node.down.height = node.height - height
        node.down.used = False
        node.down.down = NULL
        node.down.right = NULL

        node.right = <packer_node *>malloc(sizeof(packer_node))
        node.right.x = node.x + width
        node.right.y = node.y
        node.right.width = node.width - width
        node.right.height = height
        node.right.used = False
        node.right.down = NULL
        node.right.right = NULL

        return node

    @cython.cdivision(True)
    cdef packer_node *grow_node(self, unsigned int width, unsigned int height):
        cdef bint can_grow_down = width <= self.root.width
        cdef bint can_grow_right = height <= self.root.height
        # assert can_grow_down or can_grow_right, "Bad block ordering heuristic"

        cdef bint should_grow_right = ((self.root.height * self.aspect_ratio) >=
                                       (self.root.width + width))
        cdef bint should_grow_down = ((self.root.width / self.aspect_ratio) >=
                                      (self.root.height + height))

        if can_grow_right and should_grow_right:
            return self.grow_right(width, height)

        elif can_grow_down and should_grow_down:
            return self.grow_down(width, height)

        elif can_grow_right:
            return self.grow_right(width, height)

        else:
            return self.grow_down(width, height)

    cdef packer_node *grow_right(self, unsigned int width, unsigned int height):
        old_root = self.root

        self.root = <packer_node *>malloc(sizeof(packer_node))
        self.root.x = 0
        self.root.y = 0
        self.root.width = old_root.width + width
        self.root.height = old_root.height
        self.root.used = True
        self.root.down = old_root
        # self.root.right = NULL   (see below)

        self.root.right = <packer_node *>malloc(sizeof(packer_node))
        self.root.right.x = old_root.width
        self.root.right.y = 0
        self.root.right.width = width
        self.root.right.height = old_root.height
        self.root.right.used = False
        self.root.right.down = NULL
        self.root.right.right = NULL

        node = self.find_node(self.root, width, height)
        if node != NULL:
            return self.split_node(node, width, height)

    cdef packer_node *grow_down(self, unsigned int width, unsigned int height):
        old_root = self.root

        self.root = <packer_node *>malloc(sizeof(packer_node))
        self.root.x = 0
        self.root.y = 0
        self.root.width = old_root.width
        self.root.height = old_root.height + height
        self.root.used = True
        # self.root.down = NULL  (see below)
        self.root.right = old_root

        self.root.down = <packer_node *>malloc(sizeof(packer_node))
        self.root.down.x = 0
        self.root.down.y = old_root.height
        self.root.down.width = old_root.width
        self.root.down.height = height
        self.root.down.used = False
        self.root.down.down = NULL
        self.root.down.right = NULL

        node = self.find_node(self.root, width, height)
        if node != NULL:
            return self.split_node(node, width, height)


cdef struct packer_node:
    unsigned int x
    unsigned int y
    unsigned int width
    unsigned int height
    bint used
    packer_node *down
    packer_node *right
