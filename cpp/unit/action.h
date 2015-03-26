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
	/**
	 * an action radius is how close a unit must come to another
	 * unit to be considered to touch the other, for example in 
	 * gathering resource and melee attack

	 * this constructor uses the default action radius formula
	 */
	UnitAction(Unit *u, graphic_type initial_gt);

	/**
	 * used to set a specific action radius
	 */
	UnitAction(Unit *u, graphic_type initial_gt, coord::phys_t action_radius);
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
	 * action to perform when popped from a units action stack
	 */
	virtual void on_completion() = 0;

	/**
	 *	gets called for all actions on stack each update cycle
	 *	@return true when action is completed so it and everything above it can be popped
	 */
	virtual bool completed() const = 0;

	/**
	 *	checks if the action can be interrupted, allowing to be popped if the user specifies a new action
	 */
	virtual bool allow_interupt() const = 0;

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
	virtual bool allow_destruction() const = 0;

	/**
	 * debug string to identify action types
	 */
	virtual std::string name() const = 0;

	void draw_debug();

	/**
	 * common functions for actions
	 */
	void face_towards(const coord::phys3 pos);
	void damage_object(Unit &target, unsigned dmg);
	void move_to(Unit &target, bool use_range=true);

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
	 * common positional attributes
	 */
	coord::phys_t distance_to_target, radius;

	/**
	 * common graphic controls
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
class DecayAction: public UnitAction {
public:
	DecayAction(Unit *e);
	virtual ~DecayAction() {}

	void update(unsigned int) override;
	void on_completion() override;
	bool completed() const override;
	bool allow_interupt() const override { return false; }
	bool allow_destruction() const override { return false; }
	std::string name() const override { return "decay"; }

private:
	float end_frame;

};

/**
 * plays a fixed number of frames for the units dying animation
 */
class DeadAction: public UnitAction {
public:
	DeadAction(Unit *e, std::function<void()> on_complete=[]() {});
	virtual ~DeadAction() {}

	void update(unsigned int) override;
	void on_completion() override;
	bool completed() const override;
	bool allow_interupt() const override { return false; }
	bool allow_destruction() const override { return false; }
	std::string name() const override { return "dead"; }

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

	void update(unsigned int) override;
	void on_completion() override;
	bool completed() const override;
	bool allow_interupt() const override { return false; }
	bool allow_destruction() const override { return true; }
	std::string name() const override { return "idle"; }
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
	 * moves a unit to within a distance to another unit
	 */
	MoveAction(Unit *e, UnitReference tar, coord::phys_t within_range);
	virtual ~MoveAction();

	void update(unsigned int) override;
	void on_completion() override;
	bool completed() const override;
	bool allow_interupt() const override { return true; }
	bool allow_destruction() const override { return true; }
	std::string name() const override { return "move"; }

	coord::phys3 next_waypoint() const;

private:
	UnitReference unit_target;
	coord::phys3 target;
	
	path::Path path;

	// should a new path be found if unit gets blocked
	bool allow_repath;

	void initialise();

	/**
	 * use a star to find a path to target
	 */
	void set_path();

	/**
	 * updates the distance_to_target value
	 */
	void set_distance();
};

/**
 * garrison inside a building
 */
class GarrisonAction: public UnitAction {
public:
	GarrisonAction(Unit *e, UnitReference build);
	virtual ~GarrisonAction() {}

	void update(unsigned int) override;
	void on_completion() override;
	bool completed() const override { return this->complete; }
	bool allow_interupt() const override { return true; }
	bool allow_destruction() const override { return true; }
	std::string name() const override { return "garrison"; }

private:
	UnitReference building;
	bool complete;
};


/**
 * garrison inside a building
 */
class UngarrisonAction: public UnitAction {
public:
	UngarrisonAction(Unit *e, const coord::phys3 &pos);
	virtual ~UngarrisonAction() {}

	void update(unsigned int) override;
	void on_completion() override;
	bool completed() const override { return this->complete; }
	bool allow_interupt() const override { return true; }
	bool allow_destruction() const override { return true; }
	std::string name() const override { return "ungarrison"; }

private:
	coord::phys3 position;
	bool complete;
};

/**
 * trains a new unit
 */
class TrainAction: public UnitAction {
public:
	TrainAction(Unit *e, UnitProducer *pp);
	virtual ~TrainAction() {}

	void update(unsigned int) override;
	void on_completion() override;
	bool completed() const override { return this->complete; }
	bool allow_interupt() const override { return false; }
	bool allow_destruction() const override { return true; }
	std::string name() const override { return "train"; }

private:
	UnitProducer *trained;
	bool complete;
	float train_percent;
};

/**
 * builds a building
 */
class BuildAction: public UnitAction {
public:
	BuildAction(Unit *e, UnitProducer *pp, const coord::phys3 &pos);
	BuildAction(Unit *e, UnitReference foundation);
	virtual ~BuildAction() {}

	void update(unsigned int) override;
	void on_completion() override;
	bool completed() const override { return this->complete >= 1.0f; }
	bool allow_interupt() const override { return true; }
	bool allow_destruction() const override { return true; }
	std::string name() const override { return "build"; }

private:
	UnitReference building;
	UnitProducer *producer;
	coord::phys3 position;
	float complete;
};

/**
 * repairs an object
 */
class RepairAction: public UnitAction {
public:
	RepairAction(Unit *e, UnitReference tar);
	virtual ~RepairAction() {}

	void update(unsigned int) override;
	void on_completion() override;
	bool completed() const override { return this->complete; }
	bool allow_interupt() const override { return true; }
	bool allow_destruction() const override { return true; }
	std::string name() const override { return "repair"; }

private:
	UnitReference target;
	bool complete;
};

/**
 * gathers resource from another object
 */
class GatherAction: public UnitAction {
public:
	GatherAction(Unit *e, UnitReference tar);
	virtual ~GatherAction();

	void update(unsigned int) override;
	void on_completion() override;
	bool completed() const override;
	bool allow_interupt() const override { return true; }
	bool allow_destruction() const override { return true; }
	std::string name() const override { return "gather"; }

private:
	bool complete;
	UnitReference target;
	UnitReference dropsite;
};

/**
 * attacks another unit
 */
class AttackAction: public UnitAction {
public:
	AttackAction(Unit *e, UnitReference tar);
	virtual ~AttackAction();

	void update(unsigned int) override;
	void on_completion() override;
	bool completed() const override;
	bool allow_interupt() const override { return true; }
	bool allow_destruction() const override { return true; }
	std::string name() const override { return "attack"; }

private:
	UnitReference target;
	float strike_percent, rate_of_fire;

	/**
	 * use attack action
	 */
	void attack(Unit &target);

	/**
	 * add a projectile game object which moves towards the target
	 */
	void fire_projectile(const Attribute<attr_type::attack> &att, const coord::phys3 &target);
};

/**
 * convert an object
 */
class ConvertAction: public UnitAction {
public:
	ConvertAction(Unit *e, UnitReference tar);
	virtual ~ConvertAction() {}

	void update(unsigned int) override;
	void on_completion() override;
	bool completed() const override { return this->complete >= 1.0; }
	bool allow_interupt() const override { return true; }
	bool allow_destruction() const override { return true; }
	std::string name() const override { return "convert"; }

private:
	UnitReference target;
	float complete;
};

/**
 * moves object to fly in a parabolic shape
 */
class ProjectileAction: public UnitAction {
public:
	ProjectileAction(Unit *e, coord::phys3 target);
	virtual ~ProjectileAction();

	void update(unsigned int) override;
	void on_completion() override;
	bool completed() const override;
	bool allow_interupt() const override { return false; }
	bool allow_destruction() const override { return false; }
	std::string name() const override { return "projectile"; }

private:
	coord::phys_t grav;
	bool has_hit;
};

} // namespace openage

#endif
