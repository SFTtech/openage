# Copyright 2016-2021 the openage authors. See copying.md for legal info.
#
# cython: infer_types=True,profile=True

""" Routines for 2D binpacking """

# TODO pylint: disable=C,R

from enum import Enum

cimport cython

from libc.math cimport sqrt


class PackerType(Enum):
    ROW     = 0x01  # RowPacker
    COLUMN  = 0x02  # ColumnPacker
    BINTREE = 0x03  # BinaryTreePacker


@cython.boundscheck(False)
@cython.wraparound(False)
@cython.cdivision(True)
cdef inline (unsigned int, unsigned int) factor(unsigned int n):
    """Return two (preferable close) factors of n."""
    cdef unsigned int a = <unsigned int>sqrt(n)
    for num in range(a, 0, -1):
        if n % num == 0:
            return num, n // num


cdef Packer get_packer(packer_type):
    if packer_type is PackerType.ROW:
        return RowPacker
    
    elif packer_type is PackerType.COLUMN:
        return ColumnPacker
        
    elif packer_type is PackerType.BINTREE:
        return BinaryTreePacker
    
    else:
        raise Exception(f"No valid packer type: {packer_type}")


cdef class Packer:
    """
    Packs blocks.
    """
    def __init__(self, margin):
        self.margin = margin
        self.mapping = {}

    cdef void pack(self, blocks):
        """
        Pack all the blocks.

        Each block must have a width and height attribute.
        """
        raise NotImplementedError

    cdef (unsigned int, unsigned int) pos(self, block):
        return self.mapping[block]

    cdef unsigned int width(self):
        """
        Gets the total width of the packing.
        """
        return max(self.pos(block)[0] + block.width for block in self.mapping)

    cdef unsigned int height(self):
        """
        Gets the total height of the packing.
        """
        return max(self.pos(block)[1] + block.height for block in self.mapping)

    def get_packer_settings(self):
        """
        Get the init parameters set for the packer.
        """
        return (self.margin,)

    def get_mapping_hints(self, blocks):
        hints = []
        for block in blocks:
            hints.append(self.pos(block))

        return hints


cdef class DeterministicPacker(Packer):
    """
    Packs blocks based on predetermined settings.
    """

    def __init__(self, margin, hints):
        super().__init__(margin)
        self.hints = hints

    cdef void pack(self, blocks):
        for idx, block in enumerate(blocks):
            self.mapping[block] = self.hints[idx]


cdef class BestPacker:
    """
    Chooses the best result from all the given packers.
    """
    def __init__(self, packers):
        self.packers = packers
        self.current_best = None

    cdef void pack(self, blocks):
        cdef Packer p
        for packer in self.packers:
            p = packer
            p.pack(blocks)

        self.current_best = self.best_packer()

    cdef Packer best_packer(self):
        return min(self.packers, key=lambda Packer p: p.width() * p.height())

    cdef (unsigned int, unsigned int) pos(self, block):
        return self.current_best.pos(block)

    cdef unsigned int width(self):
        return self.current_best.width()

    cdef unsigned int height(self):
        return self.current_best.height()

    def get_packer_settings(self):
        return self.current_best.get_packer_settings()

    def get_mapping_hints(self, blocks):
        return self.current_best.get_mapping_hints(blocks)


cdef class RowPacker(Packer):
    """
    Packs blocks into rows, greedily trying to minimize the maximum width.
    """

    cdef void pack(self, blocks):
        self.mapping = {}

        num_rows, _ = factor(len(blocks))
        rows = [[] for _ in range(num_rows)]

        # Put blocks into rows.
        for block in blocks:
            min_row = min(rows, key=lambda row: sum(b.width for b in row))
            min_row.append(block)

        # Calculate positions.
        y = 0
        for row in rows:
            x = 0

            for block in row:
                self.mapping[block] = (x, y)
                x += block.width + self.margin

            y += max(block.height for block in row) + self.margin


cdef class ColumnPacker(Packer):
    """
    Packs blocks into columns, greedily trying to minimize the maximum height.
    """

    cdef void pack(self, blocks):
        self.mapping = {}

        num_columns, _ = factor(len(blocks))
        columns = [[] for _ in range(num_columns)]

        # Put blocks into columns.
        for block in blocks:
            min_col = min(columns, key=lambda col: sum(b.height for b in col))
            min_col.append(block)

        # Calculate positions.
        x = 0
        for column in columns:
            y = 0

            for block in column:
                self.mapping[block] = (x, y)
                y += block.height + self.margin

            x += max(block.width for block in column) + self.margin


cdef inline (unsigned int, unsigned int, unsigned int, unsigned int) maxside_heuristic(block):
    """
    Heuristic: Order blocks by maximum side.
    """
    return (max(block.width, block.height),
            min(block.width, block.height),
            block.height,
            block.width)


cdef class BinaryTreePacker(Packer):
    """
    Binary tree bin packing strategy.

    Very close to http://codeincomplete.com/posts/2011/5/7/bin_packing/.

    Aditionally can target a given aspect ratio. 97/49 is optimal for terrain
    textures.
    """
    def __init__(self, margin, aspect_ratio=1):
        # ASF: what about heuristic=max_heuristic?
        super().__init__(margin)
        self.aspect_ratio = aspect_ratio
        self.root = None

    cdef void pack(self, blocks):
        self.mapping = {}
        self.root = None

        for block in sorted(blocks, key=maxside_heuristic, reverse=True):
            self.fit(block)

    cdef (unsigned int, unsigned int) pos(self, block):
        node = self.mapping[block]
        return node.x, node.y

    def get_packer_settings(self):
        return (self.margin,)

    cdef void fit(self, block):
        if self.root is None:
            self.root = PackerNode(0, 0,
                                   block.width + self.margin,
                                   block.height + self.margin)

        node = self.find_node(self.root,
                              block.width + self.margin,
                              block.height + self.margin)

        if node is not None:
            node = self.split_node(node,
                                   block.width + self.margin,
                                   block.height + self.margin)
        else:
            node = self.grow_node(block.width + self.margin,
                                  block.height + self.margin)

        self.mapping[block] = node

    cdef PackerNode find_node(self, PackerNode root, unsigned int width, unsigned int height):
        if root.used:
            return (self.find_node(root.right, width, height) or
                    self.find_node(root.down, width, height))

        elif width <= root.width and height <= root.height:
            return root

    cdef PackerNode split_node(self, PackerNode node, unsigned int width, unsigned int height):
        node.used = True
        node.down = PackerNode(node.x, node.y + height,
                               node.width, node.height - height)
        node.right = PackerNode(node.x + width, node.y,
                                node.width - width, height)
        return node

    cdef PackerNode grow_node(self, unsigned int width, unsigned int height):
        cdef bint can_grow_down = width <= self.root.width
        cdef bint can_grow_right = height <= self.root.height
        assert can_grow_down or can_grow_right, "Bad block ordering heuristic"

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

    cdef PackerNode grow_right(self, unsigned int width, unsigned int height):
        old_root = self.root

        self.root = PackerNode(0, 0, old_root.width + width, old_root.height)
        self.root.used = True
        self.root.down = old_root
        self.root.right = PackerNode(old_root.width, 0, width, old_root.height)

        node = self.find_node(self.root, width, height)
        if node is not None:
            return self.split_node(node, width, height)

    cdef PackerNode grow_down(self, unsigned int width, unsigned int height):
        old_root = self.root

        self.root = PackerNode(0, 0, old_root.width, old_root.height + height)
        self.root.used = True
        self.root.down = PackerNode(0, old_root.height, old_root.width, height)
        self.root.right = old_root

        node = self.find_node(self.root, width, height)
        if node is not None:
            return self.split_node(node, width, height)


cdef struct packer_node:
    unsigned int x
    unsigned int y
    unsigned int width
    unsigned int height
    bint used
    packer_node * down
    packer_node * right


cdef class PackerNode:
    """
    A node in a binary packing tree.
    """
    def __cinit__(self, unsigned int x, unsigned int y, unsigned int width, unsigned int height):
        self.x = x
        self.y = y
        self.width = width
        self.height = height

        self.used = False
        self.down = None
        self.right = None
