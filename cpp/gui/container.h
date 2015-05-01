// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_CONTAINER_H_
#define OPENAGE_GUI_CONTAINER_H_

#include "control.h"
#include "forward.h"

#include <algorithm>

namespace openage {
namespace gui {

class ContainerBase : public Control {
	friend class Layout;
public:
	ContainerBase();
	virtual ~ContainerBase();

	virtual void update(seconds_t t) override;
	virtual void draw(const Drawer &drawer) const override;
	virtual bool contains(int x, int y) const override;

	virtual std::tuple<int, int> get_best_size() const override;

	virtual void mouse_left() override;
	virtual bool mouse_moved(int x, int y) override;
	virtual bool mouse_pressed(std::uint8_t button) override;
	virtual void mouse_released(std::uint8_t button) override;
	virtual bool mouse_scrolled(int relX, int relY) override;

	virtual void mouse_left_bg() {}
	virtual bool mouse_moved_bg(int /*x*/, int /*y*/) { return false; }
	virtual bool mouse_pressed_bg(std::uint8_t /*button*/) { return false; }
	virtual void mouse_released_bg(std::uint8_t /*button*/) { }
	virtual bool mouse_scrolled_bg(int /*relX*/, int /*relY*/) { return false; }

protected:
	Control *control_at(int x, int y) const;

	void set_layout(std::unique_ptr<Layout> &&layout);

	template<typename ControlType, typename... Args>
	ControlType *create(Args&&... args) {
		auto retVal = new ControlType{std::forward<Args>(args)...};
		add_control(std::unique_ptr<Control>{retVal});
		return retVal;
	}

	template<typename ControlType>
	std::unique_ptr<ControlType> remove_control(ControlType *control) {
		if (control_under_mouse == control) {
			control_under_mouse = nullptr;
		}
		auto it = std::find_if(controls.begin(), controls.end(),
		[control] (const std::unique_ptr<Control> &ptr) -> bool {
			return ptr.get() == control;
		});
		it->release(); // returns control
		controls.erase(it);
		needs_layout = true;
		return std::unique_ptr<ControlType>(control);
	}

	template<typename ControlType>
	ControlType *add_control(std::unique_ptr<ControlType> &&control) {
		ControlType *retVal = control.get();
		do_add_control(std::move(control));
		return retVal;
	}

	void do_add_control(std::unique_ptr<Control> &&control) {
		//control->invalidate_layout(); TODO
		controls.push_back(std::move(control));
		needs_layout = true;
	}

	std::vector<std::unique_ptr<Control>> clear() {
		control_under_mouse = nullptr;
		auto retVal(std::move(controls));
		retVal.clear();
		needs_layout = false;
		return retVal;
	}

	void set_controls(std::vector<std::unique_ptr<Control>> &&controls) {
		this->controls = std::move(controls);
		needs_layout = true;
	}

	const std::vector<std::unique_ptr<Control>> &get_controls() const { return controls; }

	MouseTracker mt;
	std::vector<std::unique_ptr<Control>> controls;
	std::unique_ptr<Layout> layout;
	Control *control_under_mouse = nullptr;
	int mouse_capture_count = 0;

	bool needs_layout = false;
};

class Container : public ContainerBase {
public:
	using ContainerBase::set_layout;
	using ContainerBase::create;
	using ContainerBase::add_control;
	using ContainerBase::remove_control;
};

} // namespace gui
} // namespace openage

#endif
