// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "../curve.h"

#include <experimental/any>
#include <map>
#include <memory>
#include <string>

namespace openage {
class State;
namespace curve {

class Event;
class EventManager;
class EventTarget;

/**
 * A eventclass has to be implemented for every type of event that exists.
 *
 * There shall be a genric on_X class, with a trigger_X interface for generic
 * events;
 */
class EventClass {
public:
	enum class Type {
		/// Will be triggered at the time of the event.
		/// "at" is the time, when the new value comes into action.
		/// We want to be called at this time.
		ON_CHANGE,

		/// Will be triggered immediately, when something changes.
		/// "at" is the time when something was changed.
		/// Behaves exactly like ON_CHANGE, if the ON_CHANGE is in the current
		/// execution frame.
		ON_CHANGE_IMMEDIATELY,

		/// Will be triggered on keyframe transition, when a keyframe is not
		/// relevant for the "now" execution anymore. "at" is the time of the
		/// keyframe
		ON_KEYFRAME,

		/// Will be triggered unconditionally at the set time, "at" is the
		/// time that was set as return of recalculate_time. This event will
		/// be issued again until recalculate_time returns +max(). To execute
		/// Something only once (i.E. triggered somewhere from the logic and
		/// not based on time, use ONCE

		ON_EXECUTE,

		/// Will be triggered only once, but until it is triggered the time,
		/// when this should happen can be recalculated again and again using
		/// the recalculate_time method.
		ONCE
	};

    class param_map {
    public:
        using any_t = std::experimental::fundamentals_v1::any;
        using map_t = std::map<std::string, any_t>;

        param_map(std::initializer_list<map_t::value_type> l) : map(l) {}
        param_map(const map_t& map) : map{std::move(map)} {}

        // Returns the value, if it exists and is the right type.
        // dflt if not.
        template<class _T>
        _T get(const std::string &key, const _T& dflt = _T()) const {
            auto it = this->map.find(key);
            if (it != this->map.end() && this->check_type<_T>(it)) {
                //return std::any_cast<_T>(it->second);
                return std::experimental::fundamentals_v1::any_cast<_T>(it->second);

            } else {
                return dflt;
            }
        }

        bool contains(const std::string &key) const {
            return map.find(key) != map.end();
        }

        // Check if the type is correct
        template<class _T>
        bool check_type(const std::string &key) const {
            auto it = map.find(key);
            if (it != map.end()) {
                return check_type<_T>(it);
            }
            return false;
        }

    private:
        template<class _T>
        bool check_type(const map_t::const_iterator &it) const {
            return it->second.type() == typeid(_T);
        }

    private:
        const map_t map;

    };

	/**
	 * Constructor to be constructed with the unique identifier
	 */
	EventClass(const std::string &name, Type);

	/**
	 * The Type the event class was constructed as.
	 */
	const Type type;

	/**
	 * Return a unique Identifier
	 */
	const std::string &id();

	/**
	 * To be used in the setup functionality
	 */
	void add_dependency (const std::shared_ptr<Event> &event,
	                     const std::shared_ptr<EventTarget> &dependency);

	/**
	 * The job of the setup function is to add all dependencies with other event
	 * targets found in state.
	 */
	virtual void setup(const std::shared_ptr<Event> &event,
	                   const std::shared_ptr<State> &state) = 0;

	/**
	 * will be called at the time, that was recalculated.
	 * Here all the stuff happens.
	 */
	virtual void call(EventManager *,
	                  const std::shared_ptr<EventTarget> &target,
	                  const std::shared_ptr<State> &state,
	                  const curve_time_t &t,
                      const param_map &params) = 0;

	/**
	 * will be called to recalculate, when the event shall happen.
	 * The Calculation is triggered whenever one of the set up dependencies was
	 * changed.
	 *
	 * \param target the target this was defined on
	 * \param state the state this shall work on
	 * \param at the time when the change happened, from there on it shall be
	 *           calculated onwards
	 *
	 * If the event is not relevant anymore at shall be returned as negative <curve_time_t>::max();
	 * If the time is lower than the previous time, then it may happen, that the
	 * dependencies are not perfectly resolved anymore (if other events have
	 * already been calculated before that
	 */
	virtual curve_time_t recalculate_time(const std::shared_ptr<EventTarget> &target,
	                                      const std::shared_ptr<State> &state,
	                                      const curve::curve_time_t &at)= 0;

private:
	std::string _id;
};

}} // namespace openage::curve
