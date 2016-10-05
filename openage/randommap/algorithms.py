# Copyright 2014-2016 the openage authors. See copying.md for legal info.

import classes
import constraints as c
import math
import random
import pprint
import terrain as o
import sys

def floodfill(m,islands,constraints,debug=False):
    queues = []
    for island in islands:
        queues.append(island.tiles)
        island.tiles = []

    while(sum(map(len,queues)) > 0):
        # every island
        for i in range(0,len(queues)):
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
            #tile.map.print()
            
# adds default paramenters
def loadConfiguration(config):
    c = {"GAME_SETUP":{},"MAP_SETUP":{},"LAND":[],"OBJECT":[],"CONNECTION":[]}
    
    # game setup
    c["GAME_SETUP"]["type"]    = config["GAME_SETUP"]["type"]
    c["GAME_SETUP"]["x"]       = int( config["GAME_SETUP"].get("x", 32) )
    c["GAME_SETUP"]["y"]       = int( config["GAME_SETUP"].get("y", 32) )
    c["GAME_SETUP"]["teams"]   = int( config["GAME_SETUP"].get("teams"  , 2) )
    c["GAME_SETUP"]["players"] = int( config["GAME_SETUP"].get("players", 2) )
    
    # map setup
    c["MAP_SETUP"]["base_terrain"] = o.terrain[ config["MAP_SETUP"].get("base_terrain", "GRASS") ]
    c["MAP_SETUP"]["base_x"]       = int( config["MAP_SETUP"].get("base_x", 7) )
    c["MAP_SETUP"]["base_y"]       = int( config["MAP_SETUP"].get("base_y", 7) )
    
    # lands
    for section in config.sections():
        if not section.startswith("LAND"):
            continue
        
        c["LAND"].append({
            "island_type"               : config[section].get("island_type", "circle"),
            "island_radius"             : int( config[section].get("island_radius",0) ),
            "island_tiles"              : int( config[section].get("island_tiles", 100) ),
            "space_to_other_islands"    : int( config[section].get("space_to_other_islands",0)),
            "tiles"                     : int( config[section].get("tiles",sys.maxsize)),
            "border_se"                 : float( config[section].get("border_se", 0) ),
            "border_sw"                 : float( config[section].get("border_sw", 0) ),
            "border_ne"                 : float( config[section].get("border_ne", 0) ),
            "border_nw"                 : float( config[section].get("border_nw", 0) ),
            "terrain"                   : o.terrain[ config[section].get("terrain", "GRASS") ],
            "placement"                 : config[section].get("placement", "random"),
            "placement_radius"          : float( config[section].get("placement_radius", 0.5) ),
            "placement_radius_variance" : float( config[section].get("placement_radius_variance", 0.3) ),
            "placement_angle_variance"  : float( config[section].get("placement_angle_variance", 0.3) ),
            "player_lands"              : config[section].getboolean("player_lands", False),
            "labels"                    : list( filter(lambda x: not x == '' ,config[section].get("labels","").split(",")))
        })
    
    # connections
    for section in config.sections():
        if not section.startswith("CONNECTION"):
            continue
        c["CONNECTION"].append({
            "islands"                 : list(map(str.strip,config[section].get("islands", "all_players").split(","))),
            "type"                    : config[section].get("type", "single"),
            "width"                   : int(config[section].get("width",1)),
            "substitute": {
                o.terrain["GRASS"]         : o.terrain[ config[section].get("subtitute_GRASS",      "ROAD") ],
                o.terrain["WATER"]         : o.terrain[ config[section].get("subtitute_WATER",      "SHALLOW") ],
                o.terrain["BEACH"]         : o.terrain[ config[section].get("subtitute_BEACH",      "ROAD") ],
                o.terrain["SHALLOW"]       : o.terrain[ config[section].get("subtitute_SHALLOW",    "SHALLOW") ],
                o.terrain["FORREST"]       : o.terrain[ config[section].get("subtitute_FORREST",    "ROAD") ],
                o.terrain["DIRT"]          : o.terrain[ config[section].get("subtitute_DIRT",       "ROAD") ],
                o.terrain["DEEP_WATER"]    : o.terrain[ config[section].get("subtitute_DEEP_WATER", "SHALLOW") ],
                o.terrain["ROAD"]          : o.terrain[ config[section].get("subtitute_ROAD",       "ROAD") ],
                o.terrain["ROAD_RUINED"]   : o.terrain[ config[section].get("subtitute_ROAD_RUINED","ROAD_RUINED") ],
                o.terrain["ICE"]           : o.terrain[ config[section].get("subtitute_ICE",        "ROAD") ],
            },
            "cost": {
                o.terrain["GRASS"]         : config[section].get("cost_GRASS",      1)  ,
                o.terrain["WATER"]         : config[section].get("cost_WATER",      1) ,
                o.terrain["BEACH"]         : config[section].get("cost_BEACH",      1)  ,
                o.terrain["SHALLOW"]       : config[section].get("cost_SHALLOW",    1)  ,
                o.terrain["FORREST"]       : config[section].get("cost_FORREST",    1)  ,
                o.terrain["DIRT"]          : config[section].get("cost_DIRT",       1)  ,
                o.terrain["DEEP_WATER"]    : config[section].get("cost_DEEP_WATER", 20) ,
                o.terrain["ROAD"]          : config[section].get("cost_ROAD",       1)  ,
                o.terrain["ROAD_RUINED"]   : config[section].get("cost_ROAD_RUINED",1)  ,
                o.terrain["ICE"]           : config[section].get("cost_ICE",        1)  ,
            }
        })
            
    # objects
    for section in config.sections():
        if not section.startswith("OBJECT"):
            continue
        
        c["OBJECT"].append({
            "type"                  : o.objects[ config[section].get("type", "VILLAGER") ],
            "player_id"             : int( config[section].get("player_id",0) ),
            "number"                : int( config[section].get("number", 1) ),
            "groups"                : int( config[section].get("groups", 1) ),
            "fill"                  : config[section].getboolean("fill",False),
            "chance"                : float(config[section].get("chance",0.3)),
            "islands"               : list(map(str.strip,config[section].get("islands", "all_players").split(","))),
            "min_distance"          : int( config[section].get("min_distance", 0) ),
            "max_distance"          : int( config[section].get("max_distance", 6) ),
            "obj_for_every_player"  : config[section].getboolean("obj_for_every_player", False),
            "placement"             : config[section].get("placement","random"),
        })
        c["OBJECT"][-1]["type"]["building_completion"] = float( config[section].get("building_completion", 1.0) )
    return c

def createConstraints(fullconfig,config,map,island):
    constraints = [ 
        c.Size(map,island.id,config["tiles"]),
        c.BoundingBox(int( config["border_sw"] * map.x),int( (1-config["border_ne"]) * map.x)-1,int(config["border_nw"] * map.y),int( (1-config["border_se"]) * map.y)-1,island.id),
    ]
    if config["space_to_other_islands"] > 0:
        constraints.append(c.SpaceToAllIslands(map,island.id,config["space_to_other_islands"]))
    return constraints

def createBaseIsland(map):
    island = classes.Island(0,map,0,0,terrain=map.terrain,labels=["baseland"])
    for x in range(map.x):
        for y in range(map.y):
            if map.get(x,y).island == 0:
                island.tiles.append(map.get(x,y))
    map.islands.append(island)

def createIslands(config,m):
    
    # get all islands with constraints
    islands = []
    constraints = []

    island_id = 1
    for landconfig in config["LAND"]:
        offset_minx = int( landconfig["border_sw"] * m.x)
        offset_maxx = int( (1-landconfig["border_ne"]) * m.x)-1
        offset_miny = int(landconfig["border_nw"] * m.y)
        offset_maxy = int( (1-landconfig["border_se"]) * m.y)-1
                
        if landconfig["player_lands"] == True:
            for player in range(1,config["GAME_SETUP"]["players"]+1):
                # all player will be put in a circle
                if landconfig["placement"] == "circle":
                    r = min(config["GAME_SETUP"]["x"],config["GAME_SETUP"]["y"])
                    r = r*16/2
                    maxplayers = config["GAME_SETUP"]["players"]
                    radius = landconfig["placement_radius"]
                    radius += 2*landconfig["placement_radius_variance"]*random.random()-landconfig["placement_radius_variance"]
                    radius = max(0.1,radius)
                    radius = min(0.9,radius)
                    
                    anglex = player/maxplayers+(random.random() * landconfig["placement_angle_variance"]/maxplayers)
                    angley = player/maxplayers
                    
                    x = int( r + radius * r*math.sin(2*math.pi*anglex) )
                    y = int( r + radius * r*math.cos(2*math.pi*angley) )
                else:
                    x = random.randrange(1,m.x-1)
                    y = random.randrange(1,m.y-1)
                tile         = m.get(x,y)
                tile.island  = 0
                tile.terrain = landconfig["terrain"]
                islands.append( classes.Island(
                                    island_id,m,x,y,
                                    terrain = landconfig["terrain"],
                                    labels=["_player_"+str(player)] + landconfig["labels"],
                                    tiles=[tile],
                                    player=player ))
                                    
                constraints += createConstraints(config,landconfig,m,islands[-1])                  
                island_id   += 1
        else:
            x = random.randrange(offset_minx,offset_maxx)
            y = random.randrange(offset_miny,offset_maxy)
            tile         = m.get(x,y)
            tile.island  = 0
            tile.terrain = landconfig["terrain"]
            islands.append( classes.Island(
                                island_id,m,x,y,
                                terrain=landconfig["terrain"],
                                labels=landconfig["labels"],
                                tiles=[tile]) )
            constraints += createConstraints(config,landconfig,m,islands[-1])
            island_id += 1

    m.islands.extend(islands)
    return (islands,constraints)

def printMap(filename,map):

    data  = "openage-save-file\n"
    data += "v0.1\n"
    data += "v0.3.0-454-g47d4d12\n"
    
    data += "{}\n".format( int(map.x*map.y/ 16 / 16) )
    
    # chunks
    for x_chunk in range( int(map.x / 16) ):
        for y_chunk in range( int(map.y / 16) ):
            data += "{} {}\n".format(x_chunk,y_chunk)
            data += "256\n"
            for y in range(16):
                for x in range(16):
                    tile = map.get(x_chunk*16+x,y_chunk*16+y)
                    data += "{}\n{}\n".format(tile.terrain,1)
    
    # objects
    obj_count = 0
    for x in range(map.x):
        for y in range(map.y):
            obj = map.get(x,y)
            if obj.object != None:
                obj_count +=1
    data += "{}\n".format(obj_count)
        
    for x in range(map.x):
        for y in range(map.y):
            obj = map.get(x,y)
            if obj.object != None:
                obj = obj.object
                data += "{}\n".format(obj.id)
                if obj.gaia == True:
                    data += "0\n"
                else:
                    data += "{}\n".format(obj.player_id)
                data += "{} {}\n".format(obj.x,obj.y)
                data += "{0:d}\n".format(obj.is_building)
                if obj.is_building == True:
                    data += "{}\n".format(obj.building_completion)
    
    f = open(filename, 'wb')
    f.write( bytes(data,"utf8") )

# fills an island with a an object, there is a chance it may not be placed    
def fillIsland(obj,map):
    island = map.getLandByName(obj["islands"][0])
    for tile in island.tiles:
        if random.random() <= obj["chance"]:
            if map.get(tile.x,tile.y).object == None:
                o = obj["type"]
                map.get(tile.x,tile.y).object = classes.Object(
                    id=o["id"],
                    x=tile.x,
                    y=tile.y,
                    tiles=[tile],
                    player_id = 0,
                    gaia=o["gaia"],
                )

def addObjectToIslandRandom(config,map,island):
    tile = island.getRandomTile()
    tiles = [tile]
    nonTiles = []
    while(config["number"] > 0 and len(tiles) > 0):
       
        tile = tiles.pop(random.randrange(0,len(tiles)))
        object = classes.Object(
                id=config["type"]["id"],
                x=tile.x,
                y=tile.y,
                tiles=[tile],
                player_id = config["player_id"],
                gaia=config["type"]["gaia"],
            )
            
        if not tile.isObjectPlaceable(object):
            continue

        tile.placeObject(object)
        
        for t in list(tile.getNeighbours()):
            tiles.append(t)
        config["number"] -= 1
    
def addObjectToIsland(config,map,island):
    if config["placement"] == "random":
        addObjectToIslandRandom(config,map,island)

def addObjects(config,map):
    # every object
    for obj in config["OBJECT"]:
        # filling a land/partition
        if obj["fill"] == True:
            fillIsland(obj,map)
        else:
            # how many
            for i in range(0,obj["groups"]):
                # look if for every player
                if obj["obj_for_every_player"] == True:
                    for island in map.getPlayerLands():
                        obj["player_id"] = island.player
                        addObjectToIsland(obj.copy(),map,island)
                else:
                    # place object random on islands
                    for island in obj["islands"]:
                        island = map.getLandByName(island)
                        addObjectToIsland(obj.copy(),map,island)
                        
def createConnection(config,m):
    # every connection
    for conn in config["CONNECTION"]:
        # substitute predefined labels
        if conn["type"] == "single":
            island_0 = m.getLandByName(conn["islands"][0])
            island_1 = m.getLandByName(conn["islands"][1])
            
            # calculate single tile costs
            for x in range(m.x):
                for y in range(m.y):
                    if m.get(x,y).object == None:
                        m.get(x,y).single_c = conn["cost"][m.get(x,y).terrain]
                    else:
                        m.get(x,y).single_c = sys.maxsize
                    m.get(x,y).c         = m.get(x,y).single_c
            # calculate
            for x in range(m.x):
                for y in range(m.y):
                    m.get(x,y).calcCost(conn["width"])
                    
            start = m.get(island_0.x,island_0.y)
            end   = m.get(island_1.x,island_1.y)
            path = astern(start,end,m)
            for tile in path:
                for i in range(conn["width"]):
                    m.get(tile.x+i,tile.y).terrain = conn["substitute"][m.get(tile.x+i,tile.y).terrain]
                    m.get(tile.x+i,tile.y).deleteObject()
                    m.get(tile.x,tile.y+i).terrain = conn["substitute"][m.get(tile.x,tile.y+i).terrain]
                    m.get(tile.x,tile.y+i).deleteObject()
                        
def astern(startNode,endNode,map,debug=False):
    openlist   = [startNode]
    closedlist = []
    path       = []
    if debug:
        print("{} {}".format(startNode.x,startNode.y))
        print("{} {}".format(endNode.x,endNode.y))
    while( len(openlist) != 0):
        openlist = sorted(openlist,key=lambda tile: tile.f)
        currentNode = openlist.pop(0)
            
        if currentNode.x == endNode.x and currentNode.y == endNode.y:
            # found route
            while currentNode.predecessor != None:
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
            if successor in openlist and tentative_g >= successor.g :
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

