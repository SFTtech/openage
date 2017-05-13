--[[ 
register callbacks: first row to register, second row return function
   
register Player resources Hook resource: wood,food,gold,stone type: >,<,~("any change"), returns its hook id
    hooks:onPlayerResource(std::string func, uint32_t playerID, std::string resource, double value, char type) => uint32_t
        callback: func(uint32 id, uint32 playerID, std:string(resource), double amount, double value);

register one time timer in whole seconds, returns hook its hook id
   onTime(std::string func,time_nsec_t time) => uint32_t id
    callback: func(uint32_t id, time_nsec_t gametime)
   
register intervall timer with firsttime and intervall in seconds, returns hook its hook id
   onIntervall(std::string func,time_nsec_t firsttime,time_nsec_t intervall) => uint32_t id
    callback: func(uint32_t id, time_nsec_t gametime)

register for finished buildings
   building types: CSTL, MILL, WNDR, "~" for every building
   playerID == 0 => every player
   onBuildingFinished(std::string func, uint32_t playerID, std::string building) -> uint32
    callback: func(uint32_t id, uint32_t unit_id,uint32_t playerID, buildingtype)
   
#############################
simple functions with return values: first row call function, second row return tuple
   
returns player resources playerID=0 => gaia
    getPlayerResources(uint32_t playerID)
        => (wood,food,gold,stone,pop_demand,pop_space,pop_capacity)

returns true if hook id is active, if not found or deactivated returns false
    isHookActive(uint32_t id) => bool

returns false if hook id is not found
    activateHook(uint32_t id) => bool
   
returns false if hook id is not found
    deactivateHook(uint32_t id) => bool

openage logging
    logdbg(std::string msg) -> void
    loginfo(std::string msg) -> void
    logwarn(std::string msg) -> void
    logerr(std::string msg) -> void

variable access per hook.VARIABLENAME
    version => game lua api version
    gametime => nanoseconds since gamestart
    playerCount => game player without gaia

add or deduct player resources
    changePlayerResource(uint32_t playerID, std::string resource, double value) -> void
]]
function init ()
    -- remove gold 10/s until 800
    hooks:onIntervall("gold",1,1)
    
    -- we want to be notified of any change to resources
    hooks:onPlayerResource("onResChange",1,"wood",0,"~")
    hooks:onPlayerResource("onResChange",1,"food",0,"~")
    hooks:onPlayerResource("onResChange",1,"gold",0,"~")
    hooks:onPlayerResource("onResChange",1,"stone",0,"~")
    data = {}
    data["players"] = {}
    for player = 0, (hooks.playerCount-1) do
        data["players"][player] = {}
        wood,food,gold,stone,pop_demand,pop_space,pop_capacity = hooks:getPlayerResources(player)
        data["players"][player]["wood"] = wood
        data["players"][player]["food"] = food
        data["players"][player]["gold"] = gold
        data["players"][player]["stone"] = stone
        data["players"][player]["pop_demand"] = pop_demand
        data["players"][player]["pop_space"] = pop_space
        data["players"][player]["pop_capacity"] = pop_capacity
        
    end
    
    hooks:onBuildingFinished("finishedBuilding", 0, "~")
end

function gold(a,b)
    if data["players"][1]["gold"] > 800 then
        hooks:changePlayerResource(1,"gold",-10)
    end
end

function finishedBuilding(id, unit_id, playerID, buildingtype)
    print("a player has finished a building")
    print(playerID)
    print(buildingtype)
    print("^^----^^")
end

function onResChange(id,playerID,resource,amount,value)
    if resource == "wood" then
        data["players"][playerID]["wood"] = value
    end
    if resource == "food" then
        data["players"][playerID]["food"] = value
    end
    if resource == "gold" then
        data["players"][playerID]["gold"] = value
    end
    if resource == "stone" then
        data["players"][playerID]["stone"] = value
    end
    print_r(data)
end

function print_r ( t )  
    local print_r_cache={}
    local function sub_print_r(t,indent)
        if (print_r_cache[tostring(t)]) then
            print(indent.."*"..tostring(t))
        else
            print_r_cache[tostring(t)]=true
            if (type(t)=="table") then
                for pos,val in pairs(t) do
                    if (type(val)=="table") then
                        print(indent.."["..pos.."] => "..tostring(t).." {")
                        sub_print_r(val,indent..string.rep(" ",string.len(pos)+8))
                        print(indent..string.rep(" ",string.len(pos)+6).."}")
                    elseif (type(val)=="string") then
                        print(indent.."["..pos..'] => "'..val..'"')
                    else
                        print(indent.."["..pos.."] => "..tostring(val))
                    end
                end
            else
                print(indent..tostring(t))
            end
        end
    end
    if (type(t)=="table") then
        print(tostring(t).." {")
        sub_print_r(t,"  ")
        print("}")
    else
        sub_print_r(t,"  ")
    end
    print()
end
