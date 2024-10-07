// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "input/event.h"


namespace openage {

namespace renderer::hud {
class DragRenderEntity;
class HudRenderStage;
} // namespace renderer::hud

namespace input::hud {
class BindingContext;

/**
 * Control athe HUD with input from the input manager.
 */
class Controller : public std::enable_shared_from_this<Controller> {
public:
	Controller();

	~Controller() = default;

	/**
	 * Process an input event from the input manager.
	 *
	 * @param ev Input event and arguments.
	 * @param ctx Binding context that maps input events to HUD actions.
	 *
	 * @return true if the event is accepted, else false.
	 */
	bool process(const event_arguments &ev_args,
	             const std::shared_ptr<BindingContext> &ctx);

	/**
	 * Set the render entity for the selection box.
	 *
	 * @param entity New render entity.
	 */
	void set_drag_entity(const std::shared_ptr<renderer::hud::DragRenderEntity> &entity);

	/**
	 * Get the render entity for the selection box.
	 *
	 * @return Render entity for the selection box.
	 */
	const std::shared_ptr<renderer::hud::DragRenderEntity> &get_drag_entity() const;

private:
	/**
	 * Render entity for the selection box.
	 */
	std::shared_ptr<renderer::hud::DragRenderEntity> drag_entity;
};

/**
 * Setup default HUD action bindings:
 *
 * - Mouse drag: selection box draw
 *
 * TODO: Make this configurable.
 *
 * @param ctx Binding context the actions are added to.
 * @param hud_renderer HUD render stage that is used to render the selection box.
 */
void setup_defaults(const std::shared_ptr<BindingContext> &ctx,
                    const std::shared_ptr<renderer::hud::HudRenderStage> &hud_renderer);

} // namespace input::hud
} // namespace openage
