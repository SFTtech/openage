// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_ACTION_H_
#define OPENAGE_UNIT_ACTION_H_

#include <memory>
#include <vector>

#include "../pathfinding/path.h"
#include "unit_container.h"

namespace openage {

class Texture;
class TestSound;
class Unit;

/**
 * an action to be used on the entities stack
 */
class UnitAction {
public:
	UnitAction(Unit *u, Texture *t, TestSound *s = nullptr, float fr = 0.3f);
	virtual ~UnitAction() {}

	Texture *getTex() {
		return tex;
	}

	/**
	 * use this actions graphic to draw
	 */
	void draw();

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
	 *	this action gets popped if the unit is killed
	 */
	virtual bool allow_destruction() = 0;

protected:
	/**
	 * the entity being updated
	 */
	Unit *entity;

	/**
	 * common controls
	 */
	Texture *tex;
	TestSound *on_begin;
	float frame;
	float frame_rate;
};

/**
 * plays a fixed number of frames for the units dying animation
 */
class DeadAction: public UnitAction {
public:
	DeadAction(Unit *e, Texture *t, TestSound *s = nullptr)
		:
		UnitAction( e, t, s ) {}
	virtual ~DeadAction() {}

	void update(unsigned int);
	bool completed();
	bool allow_interupt() { return false; }
	bool allow_destruction() { return false; }
};

/**
 * keeps an entity in a fixed position
 */
class IdleAction: public UnitAction {
public:
	IdleAction(Unit *e, Texture *t, TestSound *s = nullptr)
		:
		UnitAction( e, t, s ) {}
	virtual ~IdleAction() {}

	void update(unsigned int) {}
	bool completed() { return false; }
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
	MoveAction(Unit *e, Texture *t, TestSound *s, coord::phys3 tar, bool repath=true);

	/**
	 * moves a unit towards another unit
	 */
	MoveAction(Unit *e, Texture *t, TestSound *s, UnitReference tar, coord::phys_t rad);
	virtual ~MoveAction();

	void update(unsigned int);
	bool completed();
	bool allow_interupt() { return true; }
	bool allow_destruction() { return false; }
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
 * gathers resource from another object
 */
class GatherAction: public UnitAction {
public:
	GatherAction(Unit *e, UnitReference tar, Texture *t, TestSound *s);
	virtual ~GatherAction();

	void update(unsigned int);
	bool completed();
	bool allow_interupt() { return true; }
	bool allow_destruction() { return false; }

private:
	UnitReference target;
	coord::phys_t distance_to_target, range;
	float carrying;
};

/**
 * attacks another unit
 */
class AttackAction: public UnitAction {
public:
	AttackAction(Unit *e, UnitReference tar, Texture *t, TestSound *s);
	virtual ~AttackAction();

	void update(unsigned int);
	bool completed();
	bool allow_interupt() { return true; }
	bool allow_destruction() { return false; }

private:
	UnitReference target;
	coord::phys_t distance_to_target, range;
	float strike_percent;
};

} // namespace openage

#endif
