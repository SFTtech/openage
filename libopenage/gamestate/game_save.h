// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include "../engine.h"

namespace openage {

class GameMain;
class Terrain;

namespace gameio {

const std::string save_label = "openage-save-file";
const std::string save_version = "v0.2";

/**
 * a game save function that sometimes works
 */
void save(openage::GameMain *, std::string fname);

/**
 * a game load function that sometimes works
 */
void load(openage::GameMain *, std::string fname, Engine *);

}} // openage::gameio
