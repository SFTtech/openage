// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#include "stdout_logsink.h"

#include <iostream>
#include <iomanip>

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "named_logsource.h"

namespace openage {
namespace log {

namespace {


// Try to enable ansi color codes on windows machines.
void enable_ansi_color_codes() {
#ifdef _MSC_VER
	HANDLE console_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (console_output_handle == INVALID_HANDLE_VALUE) {
		return;
	}

	DWORD original_output_mode;
	if (!GetConsoleMode(console_output_handle, &original_output_mode)) {
		return;
	}

	DWORD request_output_mode = original_output_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(console_output_handle, request_output_mode);
#endif
}


}


StdOutSink::StdOutSink() {
	enable_ansi_color_codes();
}


void StdOutSink::output_log_message(const message &msg, LogSource *source) {
	// print log level (width 4)
	std::cout << "\x1b[" << msg.lvl->colorcode << "m" << std::setw(4) << msg.lvl->name << "\x1b[m" " ";

	if (msg.thread_id != 0) {
		std::cout << "\x1b[32m" "[T" << msg.thread_id << "]\x1b[m ";
	}

	if (source != &general_source()) {
		std::cout << "\x1b[36m" "[" << source->logsource_name() << "]\x1b[m ";
	}

	std::cout << msg.text << std::endl;
}


StdOutSink &global_stdoutsink() {
	static StdOutSink value;
	return value;
}


}} // namespace openage::log
