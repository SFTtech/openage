// Copyright 2025-2025 the openage authors. See copying.md for legal info.

#include "ability_usable.h"

#include <nyan/nyan.h>

#include "gamestate/game_entity.h"
#include "gamestate/api/ability.h"


namespace openage::gamestate::activity {

bool component_enabled(const time::time_t &/* time */,
                        const std::shared_ptr<gamestate::GameEntity> &entity,
                        const std::shared_ptr<gamestate::GameState> & /* state */,
                        const std::shared_ptr<nyan::Object> &condition) {
    auto ability_obj = condition->get<nyan::Object>("AbilityUsable.ability");
    auto component_type = api::APIAbility::get_component_type(*ability_obj);

    // TODO: Check if the component is enabled at time
    return entity->has_component(component_type);
}

} // namespace
