// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_ACTION_H_
#define OPENAGE_UNIT_ACTION_H_

#include <memory>
#include <vector>

#include "../pathfinding/path.h"
#include "attribute.h"
#include "unit.h"
#include "unit_container.h"

namespace openage {

/**
 * Actions can be pushed onto any units action stack
 *
 * Each update cycle will perform the update function of the
 * action on top of this stack
 */
class UnitAction {
public:
	UnitAction(Unit *u, graphic_type gt);
	virtual ~UnitAction() {}

	/**
	 * type of graphic this action should use
	 */
	graphic_type type() const;

	/**
	 * frame number to use on the current graphic
	 */
	float current_frame() const;

	/**
	 * each action has its own update functionality which gets called when this
	 * is the active action
	 */
	virtual void update(unsigned int) = 0;

	/**
	 *	gets called for all actions on stack each update cycle
	 *	@return true when action is completed so it and everything above it can be popped
	 */
	virtual bool completed() = 0;

	/**
	 *	checks if the action can be interrupted, allowing to be popped if the user specifies a new action
	 */
	virtual bool allow_interupt() = 0;

	/**
	 * control whether stack can discard the action automatically and
	 * should the stack be modifiable when this action is on top
	 *
	 * if true this action must complete and will not allow new actions
	 * to be pushed while it is active
	 * eg dead action must be completed and cannot be discarded
	 *
     * TODO: rename as allow_stack_modification
	 */
	virtual bool allow_destruction() = 0;

	void draw_debug();
	void face_towards(const coord::phys3 pos);
	void damage_object(Unit *target, unsigned dmg);

	/**
	 * produce debug info such as visualising paths
	 */
	static bool show_debug;

protected:
	/**
	 * the entity being updated
	 */
	Unit *entity;

	/**
	 * common controls
	 */
	graphic_type graphic;
	float frame;
	float frame_rate;

	/**
	 * additional drawing for debug purposes
	 */
	std::function<void(void)> debug_draw_action;
};

/**
 * plays a fixed number of frames for the units dying animation
 */
class DeadAction: public UnitAction {
public:
	DeadAction(Unit *e, std::function<void()> on_complete=[]() {});
	virtual ~DeadAction() {}

	void update(unsigned int);
	bool completed();
	bool allow_interupt() { return false; }
	bool allow_destruction() { return false; }

private:
	float end_frame;
	std::function<void()> on_complete_func;
};

/**
 * keeps an entity in a fixed position
 */
class IdleAction: public UnitAction {
public:
	IdleAction(Unit *e)
		:
		UnitAction(e, graphic_type::standing) {}
	virtual ~IdleAction() {}

	void update(unsigned int);
	bool completed();
	bool allow_interupt() { return false; }
	bool allow_destruction() { return true; }
};

/**
 * moves an entity to another location
 */
class MoveAction: public UnitAction {
public:
	/**
	 * moves unit to a given fixed location
	 */
	MoveAction(Unit *e, coord::phys3 tar, bool repath=true);

	/**
	 * moves a unit towards another unit
	 */
	MoveAction(Unit *e, UnitReference tar, coord::phys_t rad);
	virtual ~MoveAction();

	void update(unsigned int);
	bool completed();
	bool allow_interupt() { return true; }
	bool allow_destruction() { return true; }

	coord::phys3 next_waypoint() const;

private:
	UnitReference unit_target;
	coord::phys3 target;
	coord::phys_t distance_to_target, radius;
	path::Path path;

	// should a new path be found if unit gets blocked
	bool allow_repath;

	void set_path();
};

/**
 * trains a new unit
 */
class TrainAction: public UnitAction {
public:
	TrainAction(Unit *e, UnitProducer *pp);
	virtual ~TrainAction() {}

	void update(unsigned int);
	bool completed() { return complete; }
	bool allow_interupt() { return false; }
	bool allow_destruction() { return true; }

private:
	UnitProducer *trained;
	bool complete;
};

/**
 * builds a building
 */
class BuildAction: public UnitAction {
public:
	BuildAction(Unit *e, UnitProducer *pp, const coord::phys3 &pos);
	BuildAction(Unit *e, Unit *foundation);
	virtual ~BuildAction() {}

	void update(unsigned int);
	bool completed() { return complete >= 1.0f; }
	bool allow_interupt() { return true; }
	bool allow_destruction() { return true; }

private:
	Unit *building;
	UnitProducer *producer;
	coord::phys3 position;
	float complete;
	coord::phys_t range;
};

/**
 * gathers resource from another object
 */
class GatherAction: public UnitAction {
public:
	GatherAction(Unit *e, UnitReference tar);
	virtual ~GatherAction();

	void update(unsigned int);
	bool completed();
	bool allow_interupt() { return true; }
	bool allow_destruction() { return true; }

private:
	UnitReference target;
	UnitReference dropsite;
	coord::phys_t distance_to_target, range;
};

/**
 * attacks another unit
 */
class AttackAction: public UnitAction {
public:
	AttackAction(Unit *e, UnitReference tar);
	virtual ~AttackAction();

	void update(unsigned int);
	bool completed();
	bool allow_interupt() { return true; }
	bool allow_destruction() { return true; }

private:
	UnitReference target;
	coord::phys_t distance_to_target, range;
	float strike_percent, rate_of_fire;

	/**
	 * use attack action
	 */
	void attack(Unit *target);

	/**
	 * add a projectile game object which moves towards the target
	 */
	void fire_projectile(const Attribute<attr_type::attack> &att, const coord::phys3 &target);
};

/**
 * moves object to fly in a parabolic shape
 */
class ProjectileAction: public UnitAction {
public:
	ProjectileAction(Unit *e, coord::phys3 target);
	virtual ~ProjectileAction();

	void update(unsigned int);
	bool completed();
	bool allow_interupt() { return false; }
	bool allow_destruction() { return false; }

private:
	coord::phys_t grav;
	bool has_hit;
};

} // namespace openage

#endif
