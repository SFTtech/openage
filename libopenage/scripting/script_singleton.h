// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

#include "script.h"

namespace openage {

class Script;
    
class ScriptSingleton
{
public:
    static ScriptSingleton& instance() {
        static ScriptSingleton *instance = new ScriptSingleton();
        return *instance;
    }
    void addScript(Script *script) {
        this->scripts.push_back(script);
    }
    std::vector<Script*> scripts;
private:
    ScriptSingleton() {}
};

}
