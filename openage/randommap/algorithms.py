# Copyright 2014-2016 the openage authors. See copying.md for legal info.

import classes
import constraints as c
import math
import random
import pprint
import terrain as o
import sys
import itertools


def floodfill(m, islands, constraints, debug=False):
    queues = []
    for island in islands:
        queues.append(island.tiles)
        island.tiles = []

    while(sum(map(len, queues)) > 0):
        # every island
        for i in range(0, len(queues)):
            # no tiles left for island
            if len(queues[i]) == 0:
                continue
            # get random tile
            tile = queues[i].pop(random.randrange(len(queues[i])))
            # check if a constraint is violated
            if tile.island != 0:
                continue
            if False in [x.check(tile, islands[i].id) for x in constraints]:
                [x.wasAdded(False) for x in constraints]
                continue
            else:
                [x.wasAdded(True) for x in constraints]
            # set tile to island
            tile.island  = islands[i].id
            tile.terrain = islands[i].terrain
            islands[i].tiles.append(tile)

            # add new tiles to island
            queues[i].extend(tile.getNeighbours())


def makeBasesize(map):
    for island in map.islands:
        for x in range(-island.basesize, island.basesize):
            for y in range(-island.basesize, island.basesize):
                if island.x + x < 0 or island.y + y < 0 or island.x + x >= map.x or island.x + y >= map.y:
                    continue
                map.get(island.x + x, island.y + y).terrain = map.get(island.x, island.y).terrain


def deep_water(map, config):
    if config["MAP_SETUP"]["deep_water"] is False:
        return

    for x in range(map.x):
        for y in range(map.y):
            tile = map.get(x, y)
            if tile.terrain != o.terrain["WATER"]:
                continue
            radius = config["MAP_SETUP"]["deep_water_distane"]
            isDeepWater = True
            for delta_x in range(-radius, radius):
                for delta_y in range(-radius, radius):
                    # in circle?
                    if delta_x ** 2 + delta_y ** 2 <= radius ** 2:
                        continue
                    # on the map?
                    if x + delta_x < 0 or y + delta_y < 0 or y + delta_y >= map.y or x + delta_x >= map.x:
                        continue
                    tile = map.get(x + delta_x, y + delta_y)
                    # is surrounded by water or deep water?
                    if tile.terrain == o.terrain["DEEP_WATER"] or tile.terrain == o.terrain["WATER"]:
                        continue
                    isDeepWater = False
            if isDeepWater is True:
                map.get(x, y).terrain = o.terrain["DEEP_WATER"]


# adds default paramenters
def loadConfiguration(config):
    c = {"GAME_SETUP": {}, "MAP_SETUP": {}, "LAND": [], "OBJECT": [], "CONNECTION": []}

    # map setup
    c["MAP_SETUP"]["base_terrain"]        = o.terrain[config["MAP_SETUP"].get("base_terrain", "GRASS")]
    c["MAP_SETUP"]["base_x"]              = int(config["MAP_SETUP"].get("base_x", 7))
    c["MAP_SETUP"]["base_y"]              = int(config["MAP_SETUP"].get("base_y", 7))
    c["MAP_SETUP"]["x_scaling"]           = config["MAP_SETUP"].get("x_scaling", "sqrt")
    c["MAP_SETUP"]["y_scaling"]           = config["MAP_SETUP"].get("y_scaling", "sqrt")
    c["MAP_SETUP"]["deep_water"]          = config["MAP_SETUP"].getboolean("deep_water", True)
    c["MAP_SETUP"]["deep_water_distane"]  = int(config["MAP_SETUP"].get("deep_water_distance", 10))

    # game setup
    c["GAME_SETUP"]["gametype"] = config["GAME_SETUP"]["gametype"]
    c["GAME_SETUP"]["teams"]    = int(config["GAME_SETUP"].get("teams",    2))
    c["GAME_SETUP"]["players"]  = int(config["GAME_SETUP"].get("players",  2))
    c["GAME_SETUP"]["mapscale"] = int(config["GAME_SETUP"].get("mapscale", 2))
    c["GAME_SETUP"]["x"]        = int(scaling(c["MAP_SETUP"], "x", c["MAP_SETUP"]["base_x"], c["GAME_SETUP"]["mapscale"]))
    c["GAME_SETUP"]["y"]        = int(scaling(c["MAP_SETUP"], "y", c["MAP_SETUP"]["base_y"], c["GAME_SETUP"]["mapscale"]))

    # lands
    for section in config.sections():
        if not section.startswith("LAND"):
            continue

        c["LAND"].append({
            "island_type":                config[section].get("island_type", "circle"),
            "island_radius":              int(config[section].get("island_radius", 0)),
            "island_tiles":               int(config[section].get("island_tiles", 100)),
            "space_to_other_islands":     int(config[section].get("space_to_other_islands", 0)),
            "tiles":                      int(config[section].get("tiles", sys.maxsize)),
            "tiles_scaling":              config[section].get("tiles_scaling", "sqrt"),
            "basesize":                   int(config[section].get("basesize", 0)),
            "border_se":                  float(config[section].get("border_se", 0)),
            "border_sw":                  float(config[section].get("border_sw", 0)),
            "border_ne":                  float(config[section].get("border_ne", 0)),
            "border_nw":                  float(config[section].get("border_nw", 0)),
            "terrain":                    o.terrain[config[section].get("terrain", "GRASS")],
            "placement":                  config[section].get("placement", "random"),
            "placement_radius":           float(config[section].get("placement_radius", 0.5)),
            "placement_radius_variance":  float(config[section].get("placement_radius_variance", 0.3)),
            "placement_angle_variance":   float(config[section].get("placement_angle_variance", 0.3)),
            "placement_outer_radius":           float(config[section].get("placement_outer_radius", 0.5)),
            "placement_outer_radius_variance":  float(config[section].get("placement_outer_radius_variance", 0.3)),
            "player_lands":               config[section].getboolean("player_lands", False),
            "labels":                     list(filter(lambda x: not x == '', config[section].get("labels", "").split(","))),
            "polygon":                    config[section].getboolean("polygon", False),
            "polygon_points":             config[section].get("polygon_points", "").split(","),
        })
        c["LAND"][-1]["tiles"] = int(scaling(c["LAND"][-1], "tiles", c["LAND"][-1]["tiles"], c["GAME_SETUP"]["mapscale"]))

    # connections
    for section in config.sections():
        if not section.startswith("CONNECTION"):
            continue
        c["CONNECTION"].append({
            "islands":           list(map(str.strip, config[section].get("islands", "all_players").split(","))),
            "type":              config[section].get("type", "single"),
            "width":             int(config[section].get("width", 1)),
            "pave_start_island": config[section].getboolean("pave_start_island", False),
            "end_start_island":  config[section].getboolean("pave_end_island", False),
            "substitute": {
                o.terrain["GRASS"]:       o.terrain[config[section].get("subtitute_GRASS",       "ROAD")],
                o.terrain["WATER"]:       o.terrain[config[section].get("subtitute_WATER",       "SHALLOW")],
                o.terrain["BEACH"]:       o.terrain[config[section].get("subtitute_BEACH",       "ROAD")],
                o.terrain["SHALLOW"]:     o.terrain[config[section].get("subtitute_SHALLOW",     "SHALLOW")],
                o.terrain["FORREST"]:     o.terrain[config[section].get("subtitute_FORREST",     "ROAD")],
                o.terrain["DIRT"]:        o.terrain[config[section].get("subtitute_DIRT",        "ROAD")],
                o.terrain["DEEP_WATER"]:  o.terrain[config[section].get("subtitute_DEEP_WATER",  "SHALLOW")],
                o.terrain["ROAD"]:        o.terrain[config[section].get("subtitute_ROAD",        "ROAD")],
                o.terrain["ROAD_RUINED"]: o.terrain[config[section].get("subtitute_ROAD_RUINED", "ROAD_RUINED")],
                o.terrain["ICE"]:         o.terrain[config[section].get("subtitute_ICE",         "ROAD")],
            },
            "cost": {
                o.terrain["GRASS"]:       int(config[section].get("cost_GRASS",       1)),
                o.terrain["WATER"]:       int(config[section].get("cost_WATER",       1)),
                o.terrain["BEACH"]:       int(config[section].get("cost_BEACH",       1)),
                o.terrain["SHALLOW"]:     int(config[section].get("cost_SHALLOW",     1)),
                o.terrain["FORREST"]:     int(config[section].get("cost_FORREST",     1)),
                o.terrain["DIRT"]:        int(config[section].get("cost_DIRT",        1)),
                o.terrain["DEEP_WATER"]:  int(config[section].get("cost_DEEP_WATER", 20)),
                o.terrain["ROAD"]:        int(config[section].get("cost_ROAD",        1)),
                o.terrain["ROAD_RUINED"]: int(config[section].get("cost_ROAD_RUINED", 1)),
                o.terrain["ICE"]:         int(config[section].get("cost_ICE",         1)),
            },
            "object_cost": {},
        })
        # get object costs if available
        for object in o.objects:
            c["CONNECTION"][-1]["object_cost"][o.objects[object]["id"]] = int(config[section].get("cost_" + object, o.objects[object]["cost"]))

    # objects
    for section in config.sections():
        if not section.startswith("OBJECT"):
            continue

        c["OBJECT"].append({
            "type":                 o.objects[config[section].get("type", "VILLAGER")],
            "player_id":            int(config[section].get("player_id", 0)),
            "number":               int(config[section].get("number", 1)),
            "number_scaling":       config[section].get("number_scaling", "sqrt"),
            "groups":               int(config[section].get("groups", 1)),
            "groups_scaling":       config[section].get("groups_scaling", "sqrt"),
            "fill":                 config[section].getboolean("fill", False),
            "chance":               float(config[section].get("chance", 0.3)),
            "islands":              list(map(str.strip, config[section].get("islands", "all_players").split(","))),
            "min_distance":         int(config[section].get("min_distance", 0)),
            "max_distance":         int(config[section].get("max_distance", 6)),
            "obj_for_every_player": config[section].getboolean("obj_for_every_player", False),
            "placement":            config[section].get("placement", "random"),
        })
        c["OBJECT"][-1]["type"]["building_completion"] = float(config[section].get("building_completion", 1.0))
        c["OBJECT"][-1]["number"] = int(scaling(c["OBJECT"][-1], "number", c["OBJECT"][-1]["number"], c["GAME_SETUP"]["mapscale"]))
        c["OBJECT"][-1]["groups"] = int(scaling(c["OBJECT"][-1], "groups", c["OBJECT"][-1]["groups"], c["GAME_SETUP"]["mapscale"]))
    return c


def scaling(config, prefix, base_number, scaling):
    if config[prefix + "_scaling"] == "sqrt":
        return base_number * math.sqrt(scaling)
    if config[prefix + "_scaling"] == "linear":
        return base_number + scaling
    return base_number


def createConstraints(fullconfig, config, map, island):
    constraints = [
        c.Size(map, island.id, config["tiles"]),
        c.BoundingBox(int(config["border_sw"] * map.x), int((1 - config["border_ne"]) * map.x) - 1, int(config["border_nw"] * map.y), int((1 - config["border_se"]) * map.y) - 1, island.id),
    ]
    if config["space_to_other_islands"] > 0:
        constraints.append(c.SpaceToAllIslands(map, island.id, config["space_to_other_islands"]))

    if config["polygon"] is True:
        points = config["polygon_points"].copy()
        for i in range(0, len(points), 2):
            # scale relative points to mapsize
            points[i]     = int(float(points[i]) * map.x)
            points[i + 1] = int(float(points[i + 1]) * map.y)
        constraints.append(c.Polygon(points, island.id))
    return constraints


def createBaseIsland(map):
    island = classes.Island(0, map, 0, 0, terrain=map.terrain, labels=["baseland"])
    for x in range(map.x):
        for y in range(map.y):
            if map.get(x, y).island == 0:
                island.tiles.append(map.get(x, y))
    map.islands.append(island)


def createDeepPocketPlayers(config,landconfig, m, islands, constraints, island_id):
    offset_minx = int(landconfig["border_sw"] * m.x)
    offset_maxx = int((1 - landconfig["border_ne"]) * m.x) - 1
    offset_miny = int(landconfig["border_nw"] * m.y)
    offset_maxy = int((1 - landconfig["border_se"]) * m.y) - 1

    teams = m.players.teams.copy()
    random.shuffle(teams)
    players = [val for sublist in teams for val in sublist]
    # how many players per team in the inner(outer) circle
    layout = []
    inner_players = 0
    for team in teams:
        random.shuffle(team)
        if len(team) > 2:
            outer = int(round(len(team) / 2 - 0.9))
            inner = len(team) - outer
            inner_players += inner
            layout.append({"inner" : team[0:inner], "outer" : team[inner:]},)
        else:
            inner_players = len(team)
            layout.append({"inner" : team,"outer" : 0})

    r = min(m.x, m.y) / 2
    player_count  = 1
    player_offset = 1

    for team in layout:
        # maker inner ring
        player_offset = player_count
        angles_x = []
        angles_y = []
        for player in team["inner"]:
            radius = landconfig["placement_radius"]
            radius += 2 * landconfig["placement_radius_variance"] * random.random() - landconfig["placement_radius_variance"]
            radius = max(0.1, radius)
            radius = min(0.9, radius)

            anglex = player_count / inner_players + (random.random() * landconfig["placement_angle_variance"] / inner_players)
            angles_x.append(anglex)
            angley = player_count / inner_players + (random.random() * landconfig["placement_angle_variance"] / inner_players)
            angles_y.append(angley)
            player_count += 1

            x = int(r + radius * r * math.sin(2 * math.pi * anglex))
            y = int(r + radius * r * math.cos(2 * math.pi * angley))
            tile         = m.get(x, y)
            tile.island  = 0
            tile.terrain = landconfig["terrain"]
            islands.append(classes.Island(island_id,
                                        m,
                                        x,
                                        y,
                                        terrain = landconfig["terrain"],
                                        labels=["_player_" + str(player)] + landconfig["labels"],
                                        tiles=[tile],
                                        player=player,
                                        basesize=landconfig["basesize"]))

            constraints += createConstraints(config, landconfig, m, islands[-1])
            island_id   += 1
            print("{} {}".format(x,y))

        # make outer ring
        i = 0
        for player in team["outer"]:
            p_count = player_offset + i
            i += 1
            radius = landconfig["placement_outer_radius"]
            radius += 2 * landconfig["placement_outer_radius_variance"] * random.random() - landconfig["placement_outer_radius_variance"]
            radius = max(0.1, radius)
            radius = min(0.9, radius)

            anglex = (angles_x[i - 1] + angles_x[i]) / 2
            angley = (angles_y[i - 1] + angles_y[i]) / 2

            x = int(r + radius * r * math.sin(2 * math.pi * anglex))
            y = int(r + radius * r * math.cos(2 * math.pi * angley))
            tile         = m.get(x, y)
            tile.island  = 0
            tile.terrain = landconfig["terrain"]
            islands.append(classes.Island(island_id,
                                        m,
                                        x,
                                        y,
                                        terrain = landconfig["terrain"],
                                        labels=["_player_" + str(player)] + landconfig["labels"],
                                        tiles=[tile],
                                        player=player,
                                        basesize=landconfig["basesize"]))

            constraints += createConstraints(config, landconfig, m, islands[-1])
            island_id   += 1
    pprint.pprint(layout)
    return island_id

def createIslands(config, m):
    # get all islands with constraints
    islands = []
    constraints = []

    island_id = 1
    for landconfig in config["LAND"]:
        offset_minx = int(landconfig["border_sw"] * m.x)
        offset_maxx = int((1 - landconfig["border_ne"]) * m.x) - 1
        offset_miny = int(landconfig["border_nw"] * m.y)
        offset_maxy = int((1 - landconfig["border_se"]) * m.y) - 1

        # create multiple islands
        if landconfig["player_lands"] is True:
            # random circle
            players = list(range(1, m.players.players + 1))
            random.shuffle(players)

            if landconfig["placement"] == "deep_pocket":
                island_id = createDeepPocketPlayers(config, landconfig, m, islands, constraints, island_id)

            # random team_circle
            elif landconfig["placement"] == "team_circle":
                # shuffle player in team
                for team in m.players.teams:
                    random.shuffle(team)
                # get a copy, so we dont mess up the team to player assigment
                teams = m.players.teams.copy()
                random.shuffle(teams)
                players = [val for sublist in teams for val in sublist]

                for player in range(len(players)):
                    # all player will be put in a circle
                    if landconfig["placement"] == "circle" or landconfig["placement"] == "team_circle":
                        r = min(m.x, m.y) / 2
                        maxplayers = m.players.players
                        radius = landconfig["placement_radius"]
                        radius += 2 * landconfig["placement_radius_variance"] * random.random() - landconfig["placement_radius_variance"]
                        radius = max(0.1, radius)
                        radius = min(0.9, radius)
    
                        anglex = player / maxplayers + (random.random() * landconfig["placement_angle_variance"] / maxplayers)
                        angley = player / maxplayers + (random.random() * landconfig["placement_angle_variance"] / maxplayers)
    
                        x = int(r + radius * r * math.sin(2 * math.pi * anglex))
                        y = int(r + radius * r * math.cos(2 * math.pi * angley))
                    else:
                        x = random.randrange(1, m.x - 1)
                        y = random.randrange(1, m.y - 1)
                    tile         = m.get(x, y)
                    tile.island  = 0
                    tile.terrain = landconfig["terrain"]
                    islands.append(classes.Island(island_id,
                                                m,
                                                x,
                                                y,
                                                terrain = landconfig["terrain"],
                                                labels=["_player_" + str(players[player])] + landconfig["labels"],
                                                tiles=[tile],
                                                player=players[player],
                                                basesize=landconfig["basesize"]))
    
                    constraints += createConstraints(config, landconfig, m, islands[-1])
                    island_id   += 1
        # create a single island
        else:
            x = random.randrange(offset_minx, offset_maxx)
            y = random.randrange(offset_miny, offset_maxy)
            tile         = m.get(x, y)
            tile.island  = 0
            tile.terrain = landconfig["terrain"]
            island = classes.Island(island_id,
                                    m,
                                    x,
                                    y,
                                    terrain=landconfig["terrain"],
                                    labels=landconfig["labels"],
                                    tiles=[tile],
                                    basesize=landconfig["basesize"])
            islands.append(island)
            constraints_len = len(constraints)
            constraints += createConstraints(config, landconfig, m, island)

            # TODO
            # its terrible, maybe runs forever
            # we need an starttile which doesnt violates any constraint
            while(False in [x.check(tile, island_id) for x in constraints]):
                x = random.randrange(offset_minx, offset_maxx)
                y = random.randrange(offset_miny, offset_maxy)
                # set old tile to default
                tile.island = 0
                tile.terrain = m.terrain
                # new start tile
                tile = m.get(x, y)
                tile.island  = 0
                tile.terrain = landconfig["terrain"]
                island.x = x
                island.y = y
                island.tiles = [tile]
                # remove old constraints based on old x,y
                constraints = constraints[0:constraints_len]
                constraints += createConstraints(config, landconfig, m, island)

            island_id += 1

    m.islands.extend(islands)
    return (islands, constraints)


def printMap(filename, map):

    data  = "openage-save-file\n"
    data += "v0.1\n"
    data += "v0.3.0-454-g47d4d12\n"

    data += "{}\n".format(int(map.x * map.y / 16 / 16))

    # chunks
    for x_chunk in range(int(map.x / 16)):
        for y_chunk in range(int(map.y / 16)):
            data += "{} {}\n".format(x_chunk, y_chunk)
            data += "256\n"
            for y in range(16):
                for x in range(16):
                    tile = map.get(x_chunk * 16 + x, y_chunk * 16 + y)
                    data += "{}\n{}\n".format(tile.terrain, 1)

    # objects
    obj_count = 0
    for x in range(map.x):
        for y in range(map.y):
            obj = map.get(x, y)
            # x == obj...x is needed because some objects are larger than 1x1
            # so we dont count double
            if obj.object is not None and x == obj.object.x and y == obj.object.y:
                obj_count += 1
    data += "{}\n".format(obj_count)

    for x in range(map.x):
        for y in range(map.y):
            obj = map.get(x, y)
            if obj.object is not None and x == obj.object.x and y == obj.object.y:
                obj = obj.object
                data += "{}\n".format(obj.id)
                if obj.gaia is True:
                    data += "0\n"
                else:
                    data += "{}\n".format(obj.player_id)
                data += "{} {}\n".format(obj.x, obj.y)
                data += "{0:d}\n".format(obj.is_building)
                if obj.is_building is True:
                    data += "{}\n".format(obj.building_completion)

    f = open(filename, 'wb')
    f.write(bytes(data, "utf8"))


# fills an island with a an object, there is a chance it may not be placed
def fillIsland(obj, map):
    island = map.getLandByName(obj["islands"][0])
    for tile in island.tiles:
        if random.random() <= obj["chance"]:
            if map.get(tile.x, tile.y).object is None:
                o = obj["type"]
                map.get(tile.x, tile.y).object = classes.Object(
                    id=o["id"],
                    x=tile.x,
                    y=tile.y,
                    tiles=[tile],
                    player_id = 0,
                    gaia=o["gaia"],
                )


def addObjectToIslandRandom(config, map, island):
    tile = island.getRandomTile()
    tiles = [tile]
    nonTiles = []
    while(config["number"] > 0 and len(tiles) > 0):

        tile = tiles.pop(random.randrange(0, len(tiles)))
        object = classes.Object(id=config["type"]["id"],
                                x=tile.x,
                                y=tile.y,
                                x_size=config["type"]["x"],
                                y_size=config["type"]["y"],
                                tiles=[tile],
                                player_id = config["player_id"],
                                gaia=config["type"]["gaia"])

        if not tile.isObjectPlaceable(object):
            continue

        tile.placeObject(object)

        for t in list(tile.getNeighbours()):
            tiles.append(t)
        config["number"] -= 1


def addObjectToIsland(config, map, island):
    if config["placement"] == "random":
        addObjectToIslandRandom(config, map, island)


def addObjects(config, map):
    # every object
    for obj in config["OBJECT"]:
        # filling a land/partition
        if obj["fill"] is True:
            fillIsland(obj, map)
        else:
            # how many
            for i in range(0, obj["groups"]):
                # look if for every player
                if obj["obj_for_every_player"] is True:
                    for island in map.getPlayerLands():
                        obj["player_id"] = island.player
                        addObjectToIsland(obj.copy(), map, island)
                else:
                    # place object random on islands
                    for island in obj["islands"]:
                        island = map.getLandByName(island)
                        addObjectToIsland(obj.copy(), map, island)


def createSingleConnection(conn, m, island_0, island_1):
    # calculate single tile costs
    for x in range(m.x):
        for y in range(m.y):
            # cost if only terrain
            if m.get(x, y).object is None:
                m.get(x, y).single_c = conn["cost"][m.get(x, y).terrain]
            # cost of object
            else:
                m.get(x, y).single_c = conn["object_cost"][m.get(x, y).object.id]
            m.get(x, y).c         = m.get(x, y).single_c
    # calculate
    for x in range(m.x):
        for y in range(m.y):
            m.get(x, y).calcCost(conn["width"])

    start = m.get(island_0.x, island_0.y)
    end   = m.get(island_1.x, island_1.y)
    path = astern(start, end, m)

    for tile in path:
        for i in range(conn["width"]):
            m.get(tile.x, tile.y + i).deleteObject()
            m.get(tile.x + i, tile.y).deleteObject()
            if conn["pave_start_island"] is False and tile in island_0.tiles:
                # skip paving
                continue
            if conn["end_start_island"] is False and tile in island_1.tiles:
                # skip paving
                continue
            m.get(tile.x + i, tile.y).terrain = conn["substitute"][m.get(tile.x + i, tile.y).terrain]
            m.get(tile.x, tile.y + i).terrain = conn["substitute"][m.get(tile.x, tile.y + i).terrain]


def createMultipleIslandConnection(conn, m, islands):
    # travelling salesman problem: shortest path between islands
    # complexity: N!
    path = range(len(islands))
    cost = sys.maxsize
    for permutation in itertools.permutations(range(len(islands))):
        c = 0
        newpath = permutation
        for i in range(1, len(permutation)):
            island_0 = islands[permutation[i - 1]]
            island_1 = islands[permutation[i]]
            tile_0 = m.get(island_0.x, island_0.y)
            tile_1 = m.get(island_1.y, island_1.y)
            c += tile_0.distance(tile_1)
        if c <= cost:
            path = newpath

    # found path, create connection between islands
    for i in range(1, len(path)):
        createSingleConnection(conn, m, islands[i - 1], islands[i])


def createConnection(config, m):
    # every connection
    for conn in config["CONNECTION"]:

        # expand islands (predefined labels)
        if "all_players" in conn["islands"]:
            conn["islands"] = list(filter(lambda x: x != "all_players", conn["islands"]))
            for player in range(1, config["GAME_SETUP"]["players"] + 1):
                conn["islands"].append("_player_" + str(player))

        # make islands-labels unique
        conn["islands"] = list(set(conn["islands"]))

        # substitute predefined labels
        if conn["type"] == "single":
            island_0 = m.getLandByName(conn["islands"][0])
            island_1 = m.getLandByName(conn["islands"][1])
            createSingleConnection(conn, m, island_0, island_1)

        if conn["type"] == "cross":
            # make connection for all possible combinations
            for i in range(len(conn["islands"]) - 1):
                for j in range(i + 1, len(conn["islands"])):
                    island_0 = m.getLandByName(conn["islands"][i])
                    island_1 = m.getLandByName(conn["islands"][j])
                    createSingleConnection(conn, m, island_0, island_1)

        if conn["type"] == "team":
            for team in m.players.teams:
                islands = list(map(lambda playerid: m.getLandByPlayerId(playerid), team))
                createMultipleIslandConnection(conn, m, islands)


def astern(startNode, endNode, map, debug=False):
    # removed predecessor, it may have been used in an other path
    startNode.predecessor = None
    endNode.predecessor = None

    openlist   = [startNode]
    closedlist = []
    path       = []
    if debug:
        print("{} {}".format(startNode.x, startNode.y))
        print("{} {}".format(endNode.x, endNode.y))
    while(len(openlist) != 0):
        openlist = sorted(openlist, key=lambda tile: tile.f)
        currentNode = openlist.pop(0)

        if currentNode.x == endNode.x and currentNode.y == endNode.y:
            # found route
            while currentNode.predecessor is not None:
                yield currentNode
                currentNode = currentNode.predecessor
            yield(startNode)
            return
        closedlist.append(currentNode)

        # find successors
        for successor in currentNode.getNeighbours():
            if successor in closedlist:
                continue

            tentative_g = currentNode.g + successor.c
            if successor in openlist and tentative_g >= successor.g:
                continue

            successor.predecessor = currentNode
            successor.g = tentative_g

            f = tentative_g + successor.distance(endNode)
            successor.f = f
            if successor in openlist:
                index = openlist.index(successor)
                openlist[index] = successor
            else:
                openlist.append(successor)

    return None
