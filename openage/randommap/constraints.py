# Copyright 2014-2016 the openage authors. See copying.md for legal info.

from shapely.geometry import Point
from shapely.geometry.polygon import Polygon as ShapelyPolygon


class Dummy:
    # will be called bevor tile is added to island
    # only if all checks are true, tile will be added
    def check(self, tile, island):
        return True

    # will be called after decision
    # is needed for performance reasons look Class Size
    def wasAdded(self, wasAdded):
        pass


class Size:
    def __init__(self, map, island, size):
        self.island = island
        self.map = map
        self.size = size
        self.current_size = 0
        self.current_island = None

    # returns true if tile can be added to the island
    def check(self, tile, island):
        self.current_island = island
        if island != self.island:
            return True
        return self.size >= self.current_size

    def wasAdded(self, wasAdded):
        if wasAdded is True and self.current_island == self.island:
            self.current_size += 1


class SpaceToAllIslands:
    def __init__(self, map, island, space):
        self.map    = map
        self.island = island
        self.space  = space

    def check(self, tile, island):
        if island != self.island:
            return True

        for x in range(-self.space, self.space + 1):
            for y in range(-self.space, self.space + 1):
                if x == 0 and y == 0:
                    continue
                if self.map.isTile(tile.x + x, tile.y + y):
                    if self.map.get(tile.x + x, tile.y + y).island != 0 and self.map.get(tile.x + x, tile.y + y).island != island:
                        return False
        return True

    def wasAdded(self, wasAdded):
        pass


class SpaceToIsland:
    def __init__(self, map, space, island_0, island_1):
        self.space = space
        self.island_0 = island_0
        self.island_1 = island_1
        self.map = map

    # returns true if tile can be added to the island
    def check(self, tile, island):
        if not(island == self.island_0 or island == self.island_1):
            return True
        check_island = self.island_0
        if island == check_island:
            check_island = self.island_1

        # check if check_island is in range
        for x in range(-self.space, self.space + 1):
            for y in range(-self.space, self.space + 1):
                if x == 0 and y == 0:
                    continue
                if self.map.isTile(tile.x + x, tile.y + y):
                    if self.map.get(tile.x + x, tile.y + y).island == check_island:
                        return False
        return True

    def wasAdded(self, wasAdded):
        pass


class BoundingBox:
    def __init__(self, x_min, x_max, y_min, y_max, island):
        self.x_max = x_max
        self.x_min = x_min
        self.y_max = y_max
        self.y_min = y_min
        self.island = island

    def check(self, tile, island):
        if island != self.island:
            return True

        if tile.x >= self.x_min and tile.x <= self.x_max and tile.y >= self.y_min and tile.y <= self.y_max:
            return True
        return False

    def wasAdded(self, wasAdded):
        pass


class Polygon:
    def __init__(self, points, island):
        self.polygon = ShapelyPolygon(points)
        self.island = island

    def check(self, tile, island):
        if island != self.island:
            return True
        return self.polygon.contains(Point((tile.x, tile.y)))

    def wasAdded(self, wasAdded):
        pass
