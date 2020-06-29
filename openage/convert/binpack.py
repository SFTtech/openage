# Copyright 2016-2016 the openage authors. See copying.md for legal info.

""" Routines for 2D binpacking """

# TODO pylint: disable=C,R


import math


def factor(n):
    """Return two (preferable close) factors of n."""
    for a in range(int(math.sqrt(n)), 0, -1):
        if n % a == 0:
            return a, n // a


class Packer:
    """Packs blocks."""

    def __init__(self, margin):
        self.margin = margin
        self.mapping = {}

    def pack(self, blocks):
        """
        Pack all the blocks.

        Each block must have a width and height attribute.
        """
        raise NotImplementedError

    def pos(self, block):
        return self.mapping[block]

    def width(self):
        """Gets the total width of the packing."""
        return max(self.pos(block)[0] + block.width for block in self.mapping)

    def height(self):
        """Gets the total height of the packing."""
        return max(self.pos(block)[1] + block.height for block in self.mapping)


class RowPacker(Packer):
    """
    Packs blocks into rows, greedily trying to minimize the maximum width.
    """

    def pack(self, blocks):
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


class ColumnPacker(Packer):
    """
    Packs blocks into columns, greedily trying to minimize the maximum height.
    """

    def pack(self, blocks):
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


class BestPacker:
    """Chooses the best result from all the given packers."""

    def __init__(self, packers):
        self.packers = packers
        self.current_best = None

    def pack(self, blocks):
        for packer in self.packers:
            packer.pack(blocks)

        self.current_best = self.best_packer()

    def best_packer(self):
        return min(self.packers, key=lambda p: p.width() * p.height())

    def pos(self, block):
        return self.current_best.pos(block)

    def width(self):
        return self.current_best.width()

    def height(self):
        return self.current_best.height()


class PackerNode:
    """A node in a binary packing tree."""

    def __init__(self, x, y, width, height):
        self.x = x
        self.y = y
        self.width = width
        self.height = height

        self.used = False
        self.down = None
        self.right = None


def maxside_heuristic(block):
    """Heuristic: Order blocks by maximum side."""
    return (max(block.width, block.height),
            min(block.width, block.height),
            block.height,
            block.width)


class BinaryTreePacker(Packer):
    """
    Binary tree bin packing strategy.

    Very close to http://codeincomplete.com/posts/2011/5/7/bin_packing/.

    Aditionally can target a given aspect ratio. 97/49 is optimal for terrain
    textures.
    """

    def __init__(self, margin, aspect_ratio=1, heuristic=maxside_heuristic):
        super().__init__(margin)
        self.aspect_ratio = aspect_ratio
        self.heuristic = heuristic
        self.root = None

    def pack(self, blocks):
        self.mapping = {}
        self.root = None

        for block in sorted(blocks, key=self.heuristic, reverse=True):
            self.fit(block)

    def pos(self, block):
        node = self.mapping[block]
        return node.x, node.y

    def fit(self, block):
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

    def find_node(self, root, width, height):
        if root.used:
            return (self.find_node(root.right, width, height) or
                    self.find_node(root.down, width, height))
        elif width <= root.width and height <= root.height:
            return root

    def split_node(self, node, width, height):
        node.used = True
        node.down = PackerNode(node.x, node.y + height,
                               node.width, node.height - height)
        node.right = PackerNode(node.x + width, node.y,
                                node.width - width, height)
        return node

    def grow_node(self, width, height):
        can_grow_down = width <= self.root.width
        can_grow_right = height <= self.root.height
        assert can_grow_down or can_grow_right, "Bad block ordering heuristic"

        should_grow_right = ((self.root.height * self.aspect_ratio) >=
                             (self.root.width + width))
        should_grow_down = ((self.root.width / self.aspect_ratio) >=
                            (self.root.height + height))

        if can_grow_right and should_grow_right:
            return self.grow_right(width, height)
        elif can_grow_down and should_grow_down:
            return self.grow_down(width, height)
        elif can_grow_right:
            return self.grow_right(width, height)
        else:
            return self.grow_down(width, height)

    def grow_right(self, width, height):
        old_root = self.root

        self.root = PackerNode(0, 0, old_root.width + width, old_root.height)
        self.root.used = True
        self.root.down = old_root
        self.root.right = PackerNode(old_root.width, 0, width, old_root.height)

        node = self.find_node(self.root, width, height)
        if node is not None:
            return self.split_node(node, width, height)

    def grow_down(self, width, height):
        old_root = self.root

        self.root = PackerNode(0, 0, old_root.width, old_root.height + height)
        self.root.used = True
        self.root.down = PackerNode(0, old_root.height, old_root.width, height)
        self.root.right = old_root

        node = self.find_node(self.root, width, height)
        if node is not None:
            return self.split_node(node, width, height)
