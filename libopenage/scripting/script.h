// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

#include "../util/timing.h"
#include "../gamestate/game_main.h"
#include "../gamestate/resource.h"
#include "../unit/unit.h"
#include "sol.hpp"

namespace openage {

class GameMain;
struct Hooks;

class Script{
    
public:
    Script(std::string scriptfile, GameMain *game);
    ~Script();
    void update(time_nsec_t lastframe_duration,time_nsec_t gametime);
    
    bool setStatus(uint32_t id,bool status);
    bool getStatus(uint32_t id);
    uint32_t register_onTime(std::string func,time_nsec_t time);
    uint32_t register_onIntervall(std::string func,time_nsec_t nexttime,time_nsec_t intervall);
    
    uint32_t register_onPlayerResource(std::string func, uint32_t playerID, std::string resource, double value, char type);
    void onResourceChange(uint32_t playerID, game_resource resource, double amount, double value);
    
    uint32_t register_onBuildingFinished(std::string func, uint32_t playerID, std::string building);
    void onBuildingFinished(Unit *unit, uint32_t id, std::string name);
    
    uint32_t callbacks;
private:
    void init(std::string scriptfile, GameMain *game);
    struct Timer{
        uint32_t id;
        time_nsec_t time;
        sol::function func;
        bool active;
    };
    struct TimerIntervall{
        uint32_t id;
        time_nsec_t nexttime;
        time_nsec_t intervall;
        sol::function func;
        bool active;
    };
    struct BuildingFinished {
        uint32_t id;
        uint32_t playerID;
        std::string building;
        sol::function func;
        bool active;
    };
    GameMain *game;
    Hooks *hooks;
    uint64_t gametime;
    sol::state *script;
    /*
     * registeres callbacks
     */
    std::vector<Timer> timer;
    std::vector<TimerIntervall> timerIntervall;
    std::vector<BuildingFinished> buildingFinished;
    std::vector<uint32_t> buildingFinishedIds;
public:
    struct ResourceChange{
        uint32_t id;
        uint32_t playerID;
        game_resource resource;
        char type;
        double value;
        sol::function func;
        bool active;
    };
    std::vector<ResourceChange> resourceChange;
};
}
