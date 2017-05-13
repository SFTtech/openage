// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#include "script.h"
#include "../log/log.h"
#include "../gamestate/player.h"
#include "../gamestate/civilisation.h"
#include "../gamestate/population_tracker.h"
#include <iostream>
#include <exception>
#include "../unit/unit.h"
#include "../unit/unit_container.h"
#include "../unit/unit_type.h"

namespace openage {
    
 
struct Hooks {
    public:
        time_nsec_t gametime = 0;
        time_nsec_t lastframe_duration = 0;
        uint8_t version = 1;
        uint8_t player_count = 0;
        GameMain *game;
        Script *script;
        sol::state *luastate;
        
        // player resources
        std::tuple<double,double,double,double,int,int,int>
        getPlayerResources(uint32_t playerID) { return std::make_tuple(
                        game->players[playerID].amount(game_resource::wood),
                        game->players[playerID].amount(game_resource::food),
                        game->players[playerID].amount(game_resource::gold),
                        game->players[playerID].amount(game_resource::stone),
                        game->players[playerID].population.get_demand(),
                        game->players[playerID].population.get_space(),
                        game->players[playerID].population.get_capacity()
                        ); };
        std::tuple<int,std::string,int> 
        getPlayerInfo(uint32_t playerID) { 
            auto player = game->get_player(playerID); 
            return std::make_tuple( player->player_number,player->name,player->civ->civ_id); };
        
        uint32_t onPlayerResource(std::string func, uint32_t playerID, std::string resource, double value, char type) {
            return script->register_onPlayerResource(func,playerID,resource,value,type);
        };
        void changePlayerResource(uint32_t playerID, std::string resource, double value) {
            auto res = game_resource::wood;
            if (resource == "food")  { res=game_resource::food; }
            if (resource == "gold")  { res=game_resource::gold; }
            if (resource == "stone") { res=game_resource::stone; }
            if(value >=0) {
                game->get_player(playerID)->receive(res,value);
            }
            game->get_player(playerID)->deduct(res,-1*value);
        };
        
        // log stuff for scripts
        void logdbg  (std::string msg){ log::log(MSG(dbg)  << msg); };
        void loginfo (std::string msg){ log::log(MSG(info) << msg); };
        void logwarn (std::string msg){ log::log(MSG(warn) << msg); };
        void logerr  (std::string msg){ log::log(MSG(err)  << msg); };
        
        // get hooks activation status or set it
        bool isHookActive(uint32_t id){ return script->getStatus(id); };
        bool activateHook(uint32_t id){ return script->setStatus(id,true); };
        bool deactivateHook(uint32_t id){ return script->setStatus(id,false); };

        // building hooks
        uint32_t onBuildingFinished(std::string func, uint32_t playerID, std::string building){ 
            return script->register_onBuildingFinished(func,playerID,building);
        };

        // time hooks
        uint32_t onTime(std::string func,time_nsec_t time){
            return script->register_onTime(func, time);
        };
        uint32_t onIntervall(std::string func,time_nsec_t nexttime,time_nsec_t intervall){
            return script->register_onIntervall(func,nexttime,intervall);
        };
        
        // winner, looser hooks
        void playerLooses(uint32_t playerID) { log::log(MSG(info) << game->get_player(playerID)->name << " looses game"); };
        void playerWins(uint32_t playerID) {log::log(MSG(info) << game->get_player(playerID)->name << " wins game"); };
};

uint32_t Script::register_onPlayerResource(std::string func, uint32_t playerID, std::string resource, double value, char type) {
    /*
    * resource: food, wood, gold, stone,
    * status:   ~(any change), TODO: >=, <=,
    */
    auto res = game_resource::wood;
    if (resource == "food")  { res=game_resource::food; }
    if (resource == "gold")  { res=game_resource::gold; }
    if (resource == "stone") { res=game_resource::stone; }
    sol::state_view luascript( *(this->script) );
    this->callbacks++;
    this->resourceChange.push_back(Script::ResourceChange{this->callbacks,playerID,res, type, value, luascript[func],true});
    return (this->callbacks-1);
}

void Script::onResourceChange(uint32_t playerID, game_resource resource, double amount, double value) {
    for(auto hook : this->resourceChange) {
       
        if(hook.active == true && hook.playerID == playerID && hook.resource == resource) {
            if( hook.type == '~' || (hook.type == '>' && value >= hook.value) || (hook.type == '<' && value <= hook.value) ) {
                hook.func(hook.id, playerID, std::to_string(resource), amount, value);
            }
        }
    }
}

uint32_t Script::register_onBuildingFinished(std::string func, uint32_t playerID, std::string building) {
    sol::state_view luascript( *(this->script) );
    this->callbacks++;
    this->buildingFinished.push_back(BuildingFinished{this->callbacks,playerID,building,luascript[func],true});
    return (this->callbacks-1);
}

void Script::onBuildingFinished(Unit *unit, uint32_t id, std::string name) {
    /*
     * every building villager sends a callback, so we have to filter
     */
    for(auto tmp_id : this->buildingFinishedIds) {
        if(id == tmp_id) {
            return;
        }
    }
    for(auto &building : this->buildingFinished) {
        // check user
        if(building.playerID == id || building.playerID == 0) {
            // check building type
            if(building.building == "~" || building.building == name) {
                this->buildingFinishedIds.push_back(id);
                // find user id of owning player
                for(auto &player : this->game->players) {
                    if(player.owns(*unit)) {
                        building.func(building.id,id,player.color,name);
                    }
                }
            }
        }
    }
}

uint32_t Script::register_onIntervall(std::string func,time_nsec_t nexttime,time_nsec_t intervall) {
    sol::state_view script( *(this->script));
    // fill callback struct
    this->timerIntervall.push_back(
        TimerIntervall{this->callbacks,nexttime*1000000000,intervall*1000000000,script[func],true}
    );
    this->callbacks++;
    return this->callbacks-1;
}

uint32_t Script::register_onTime(std::string func,time_nsec_t time) {
    sol::state_view script( *(this->script));
    // fill callback struct
    this->timer.push_back(
        Timer{this->callbacks,time*1000000000,script[func],true}
    );
    this->callbacks++;
    return this->callbacks-1;
}

Script::Script(std::string scriptfile, GameMain *game) {
    this->game = game;
    this->callbacks= 0;
    this->init(scriptfile, game);
}
void Script::init(std::string scriptfile, GameMain *game) {
    
    /*
     * register this instance to player
     */
    for( auto &player : game->players) {
        player.script_hooks.push_back(this);
    }

    this->script = new sol::state();
    
    sol::state_view script( *(this->script));
    
    // libraries: base,package,coroutine,string,os,math,table,debug,bit32,io,ffi,jit,utf8,count
    script.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string, sol::lib::utf8);
    try {
        script.script_file(scriptfile);
        
        // call init function
        script.open_libraries(sol::lib::base, sol::lib::package);
        // open script from disk
        script.script_file(scriptfile);
        // create oa namespace
        sol::table oa = script.create_named_table("oa","version",1);
        
        // populate callbacks
        script.new_usertype<Hooks> ("hooks",
            "version",  sol::readonly( &Hooks::version ),
            "gametime", sol::readonly( &Hooks::gametime ),
            "lastframe_duration", sol::readonly( &Hooks::lastframe_duration ),
            "playerCount", sol::readonly( &Hooks::player_count ),
            "logdbg",   &Hooks::logdbg,
            "loginfo",  &Hooks::loginfo,
            "logwarn",  &Hooks::logwarn,
            "logerr",   &Hooks::logerr,
            "onPlayerResource",   &Hooks::onPlayerResource,
            "getPlayerResources", &Hooks::getPlayerResources,
            "changePlayerResource", &Hooks::changePlayerResource,
            
            // activate deactivate hooks
            "activateHook",   &Hooks::activateHook,
            "deactivateHook", &Hooks::deactivateHook,
            "isHookActive",   &Hooks::isHookActive,
            
            // buildings stuff
            "onBuildingFinished", &Hooks::onBuildingFinished,
            
            // register timing Hooks
            "onTime",      &Hooks::onTime,
            "onIntervall", &Hooks::onIntervall,
            
            // win/looser
            "playerLooses", &Hooks::playerLooses,
            "playerWins",   &Hooks::playerWins
        );
        
        this->hooks = new Hooks();
        this->hooks->game = this->game;
        this->hooks->script = this;
        this->hooks->luastate = this->script;
        this->hooks->player_count = game->player_count();
        script["hooks"] = this->hooks;
            
        // call script initialisation function
        sol::function init = script["init"];
        init();
    } catch (std::exception& e) {
        log::log(MSG(err) << "script " << scriptfile << " failed");
        log::log(MSG(err) << e.what());
    }
}



bool Script::setStatus(uint32_t id,bool status) {
    for(auto &timer : this->timer) {
        if (timer.id == id) {
            timer.active = status;
            return true;
        }
    }
    for(auto &timer : this->timerIntervall) {
        if (timer.id == id) {
            timer.active = status;
            return true;
        }
    }
    for(auto &building : this->buildingFinished) {
        if (building.id == id) {
            building.active = status;
            return true;
        }
    }
    for(auto &resource : this->resourceChange) {
        if (resource.id == id) {
            resource.active = status;
            return true;
        }
    }
    return false;
}

bool Script::getStatus(uint32_t id) {
    for(auto &timer : this->timer) {
        if (timer.id == id) {
            return timer.active;
        }
    }
    for(auto &timer : this->timerIntervall) {
        if (timer.id == id) {
            return timer.active;
        }
    }
    for(auto &resource : this->resourceChange) {
        if (resource.id == id) {
            return resource.active;
        }
    }
    for(auto &building : this->buildingFinished) {
        if (building.id == id) {
            return building.active;
        }
    }
    return false;
}

Script::~Script() {
    // free memory
    //delete this->hooks;
}

void Script::update(time_nsec_t lastframe_duration,time_nsec_t gametime) {
    this->hooks->gametime = gametime;
    this->hooks->lastframe_duration = lastframe_duration;
    // check timer callbacks
    for(auto &timer : this->timer) {
        if(timer.time <= gametime && timer.active == true) {
            // execute callbacks
            timer.active = false;
            timer.func(timer.id,gametime);
        }
    }
    
    //check timer intervall callbacks
    for(auto &timer : this->timerIntervall) {
        if(timer.nexttime <= gametime && timer.active == true) {
            // execute callbacks
            try { timer.func(timer.id,gametime); } catch (...) { log::log(MSG(err) << "lua error");}
            timer.nexttime += timer.intervall;
        }
    }
}

}
