// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GUI_SIGNAL_H_
#define OPENAGE_GUI_SIGNAL_H_

#include <functional>
#include <vector>

namespace openage {
namespace gui {

template<typename... Args>
class Signal {
public:
	using function_t = std::function<void(Args...)>;

	void connect(const function_t &handler) {
		connections.push_back(handler);
	}

	void emit(const Args &... args) {
		for (auto &handler : connections) {
			handler(args...);
		}
	}

protected:
	std::vector<function_t> connections;
};

} // namespace gui
} // namespace openage

#endif
