# Copyright 2014-2016 the openage authors. See copying.md for legal info.

import random
import sys
import terrain as o


class Map:
    def __init__(self, x, y, terrain=0, players=None):
        self.x       = x * 16
        self.y       = y * 16
        self.players = players
        self.terrain = terrain
        self.islands = []
        self.data    = [[Tile(xx, yy, self, island=0, terrain=self.terrain) for yy in range(self.y)] for xx in range(self.x)]

    def get(self, x, y):
        if x >= self.x or y >= self.y or x < 0 or y < 0:
            return None
        return self.data[x][y]

    def set(self, x, y, value):
        self.data[x][y] = value

    def print(self):
        for y in range(0, self.y):
            for x in range(0, self.x):
                tile = self.get(x, y)
                if tile.object is None:
                    if tile.island == 0 and tile.terrain == self.terrain:
                        print("_", end="")
                    else:
                        if tile.terrain == o.terrain["GRASS"]:
                            print("G", end="")
                        elif tile.terrain == o.terrain["SHALLOW"]:
                            print("S", end="")
                        elif tile.terrain == o.terrain["ROAD"]:
                            print("R", end="")
                        else:
                            print("{:1}".format(tile.island), end="")
                else:
                    if tile.object.id == o.objects["TREE1"]["id"]:
                        print("{:1}".format('T'), end="")
                    elif tile.object.id == o.objects["GOLD"]["id"]:
                        print("{:1}".format('G'), end="")
                    elif tile.object.id == o.objects["STONE"]["id"]:
                        print("{:1}".format('S'), end="")
                    elif tile.object.id == o.objects["VILLAGER"]["id"]:
                        print("{:1}".format('V'), end="")
                    elif tile.object.id == o.objects["TOWNCENTER"]["id"]:
                        print("{:1}".format('T'), end="")
                    else:
                        print("{:1}".format('o'), end="")
            print()
        print()

    def isTile(self, x, y):
        if x >= 0 and y >= 0 and x < self.x and y < self.y:
            return True
        return False

    # returns all Neighbours of a tile
    def getNeighbours(self, x, y):
        for i in [-1, 0, 1]:
            for j in [-1, 0, 1]:

                if self.isTile(x + i, y + j):
                    if (i == 0 or j == 0) and not (i == 0 and j == 0):
                        if self.isTile(x + i, y + j):
                            yield(self.get(x + i, y + j))

    def getPlayerLands(self):
        for island in self.islands:
            if island.player > 0:
                yield(island)

    def getLandByName(self, name):
        for island in self.islands:
            if name in island.labels:
                return island

    def getLandByPlayerId(self, playerid):
        for island in self.islands:
            if island.player == playerid:
                return island

        return None


class Players:
    def __init__(self, players):
        self.players = len(players)
        team_count = len(list(set(players)))
        self.teams = [[] for i in range(team_count)]
        for i in range(self.players):
            self.teams[players[i]].append(i + 1)


class Tile:
    def __init__(self, x, y, m, island=0, terrain=0, object=None):
        self.x       = x
        self.y       = y
        self.map     = m
        self.island  = island
        self.terrain = terrain
        self.object  = object
        # for astern algorithm
        # cost of this tile
        self.c = 0
        self.single_c = 0
        # cost to come to this tile
        self.g = 0
        # cost to end point with approximation function
        self.f = 0
        self.predecessor = None

    def isObjectPlaceable(self, object):
        for x in range(self.x, self.x + object.x_size):
            for y in range(self.y, self.y + object.y_size):
                if x >= self.map.x or y >= self.map.y or self.map.get(x, y).object is not None:
                    return False
        return True

    def placeObject(self, object):
        object.tiles = []
        for x in range(self.x, self.x + object.x_size):
            for y in range(self.y, self.y + object.y_size):
                object.tiles.append(self.map.get(x, y))
                self.map.get(x, y).object = object

    def deleteObject(self):
        self.object = None

    def distance(self, other):
        # euklid distance without squareroot
        return (self.x - other.x) ** 2 + (self.y - other.y) ** 2

    # returns the cost of tiles in x and y axis
    def calcCost(self, width):
        for i in range(width):
            if self.x + i < self.map.x:
                self.c += self.map.get(self.x + i, self.y).c
            else:
                self.c += sys.maxsize

            if self.y + i < self.map.y:
                self.c += self.map.get(self.x, self.y + i).c
            else:
                self.c += sys.maxsize

    def getNeighbours(self):
        return self.map.getNeighbours(self.x, self.y)


class Island:
    def __init__(self, id, map, x, y, terrain, labels=[], tiles=[], player=0):
        self.id = id
        self.player = player
        self.labels = labels
        self.terrain = terrain
        self.map = map
        self.tiles = tiles
        # startpoint for expansion
        self.x = x
        self.y = y

    def getRandomTile(self):
        return self.tiles[random.randrange(0, len(self.tiles))]


class Object:
    def __init__(self, id=None, x=-1, y=-1, x_size=1, y_size=1, is_building=False, building_completion=1.0, gaia=False, player_id=0, tiles=[]):
        self.tiles               = tiles
        self.id                  = id
        self.player_id           = player_id
        self.x                   = x
        self.y                   = y
        self.x_size              = x_size
        self.y_size              = y_size
        self.is_building         = is_building
        self.building_completion = building_completion
        self.gaia                = gaia
