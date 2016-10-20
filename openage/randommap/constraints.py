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
        self.polygon = list(map(int, points))
        self.polygon.append(self.polygon[0])
        self.polygon.append(self.polygon[1])
        self.island = island

    def pointInPolygon(self, p_x, p_y):
        t = -1
        for i in range(0, len(self.polygon) - 4, 2):
            t = t * self.test(p_x, p_y, self.polygon[i], self.polygon[i + 1], self.polygon[i + 2], self.polygon[i + 3])
        if t == -1:
            return False
        return True

    # https://de.wikipedia.org/wiki/Punkt-in-Polygon-Test_nach_Jordan
    def test(self, x_A, y_A, x_B, y_B, x_C, y_C):
        if y_A == y_B and y_B == y_C:
            if (x_B <= x_A and x_A <= x_C) or (x_C <= x_A and x_A <= x_B):
                return 0
            else:
                return 1

        if y_B > y_C:
            # swap B and C
            tmp = x_B
            x_B = x_C
            x_C = tmp
            tmp = y_B
            y_B = y_C
            y_C = tmp

        if y_A == y_B and x_A == x_B:
            return 0

        if y_A <= y_B or y_A > y_C:
            return 1
        delta = (x_B - x_A) * (y_C - y_A) - (y_B - y_A) * (x_C - x_A)
        if delta > 0:
            return -1
        elif delta < 0:
            return 1
        else:
            return 0

    def check(self, tile, island):
        if island != self.island:
            return True
        return self.pointInPolygon(tile.x, tile.y)

    def wasAdded(self, wasAdded):
        pass
