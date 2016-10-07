# Copyright 2014-2016 the openage authors. See copying.md for legal info.

import algorithms as algo
import classes
import constraints
import configparser
import random
import pprint
import sys
import time


# this should be called from the game
# @params
# players: list of player teams, [0,1,1]
#           player 1 and 3 are in team 0
#           player 2 is in team 1
def generate(filename, players, gametype, mapscale, seed):

    # rng seed
    random.seed(seed)

    # load config
    config = configparser.ConfigParser()
    config.read(filename, "utf8")
    config = algo.loadConfiguration(config)

    # add parameter from ingame
    config["GAME_SETUP"]["players"]  = len(players)
    config["GAME_SETUP"]["x"]        = config["MAP_SETUP"]["base_x"] + mapscale
    config["GAME_SETUP"]["y"]        = config["MAP_SETUP"]["base_y"] + mapscale
    config["GAME_SETUP"]["mapscale"] = mapscale
    config["GAME_SETUP"]["gametype"] = gametype

    # make teams
    # TODO

    # create map and tiles
    map = classes.Map(config["GAME_SETUP"]["x"], config["GAME_SETUP"]["y"], terrain=config["MAP_SETUP"]["base_terrain"])

    # generate islands startpoints and constraints
    (islands, constraints) = algo.createIslands(config, map)

    # radom floodfill (island expansion)
    algo.floodfill(map, islands, constraints)

    # all tiles which are not assigned to an island is a baseland
    algo.createBaseIsland(map)

    # place objects(trees,units,building) on terrain
    algo.addObjects(config, map)

    # create connections
    algo.createConnection(config, map)

    # create savefile
    algo.printMap("/tmp/default_save.oas", map)

    map.print()

if __name__ == "__main__":

    generate("maps/arabia.rms", [0, 0, 0, 0, 0, 0], "conquest", 6, 42)
