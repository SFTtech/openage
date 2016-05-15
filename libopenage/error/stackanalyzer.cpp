// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "stackanalyzer.h"

#include <libopenage/config.h>

#include "../log/log.h"
#include "../util/compiler.h"
#include "../util/init.h"


#if WITH_BACKTRACE

// use modern <backtrace.h>
#include <backtrace.h>

namespace openage {
namespace error {

// those functions are for internal usage only.
namespace {


struct info_cb_data_t {
	std::vector<backtrace_symbol> symbols;
	uintptr_t pc;
};


// called in case of internal errors of libbacktrace
void backtrace_error_callback(void * /*unused*/, const char *msg, int errorno) {
	log::log(ERR << "libbacktrace: " << msg << " (errno: " << errorno << ")");
}


struct backtrace_state *bt_state;


util::OnInit init_backtrace_state([]() {
	bt_state = backtrace_create_state(
		nullptr,                    // auto-determine filename
		1,                          // threaded
		backtrace_error_callback,
		nullptr                     // passed to the callback
	);

	// There's no documentaton on how to free the state.
});


// called by backtrace_simple in StackAnalyzer::collect()
int backtrace_simple_callback(void *data, uintptr_t pc) {
	// data is a pointer to the StackAnalyzer object
	StackAnalyzer *bt = reinterpret_cast<StackAnalyzer *>(data);
	bt->stack_addrs.push_back(reinterpret_cast<void *>(pc));
	return 0;
}


void backtrace_syminfo_callback(
	void *data,
	uintptr_t pc,
	const char *symname,
	uintptr_t /*unused symval*/,
	uintptr_t /*unused symsize*/) {

	// in this fallback case, we can't get filename or line info, but at least we
	// can get and demangle the symbol name... hopefully.

	auto symbol_vector = &(reinterpret_cast<info_cb_data_t *>(data)->symbols);

	symbol_vector->emplace_back(backtrace_symbol{
		"",
		0,
		(symname == nullptr) ? "" : util::demangle(symname),
		reinterpret_cast<void *>(pc)
	});
}


// called by backtrace_pcinfo in StackAnalyzer::get_symbols
int backtrace_pcinfo_callback(void *data, uintptr_t pc, const char *filename, int lineno, const char *function) {
	backtrace_symbol result;

	auto symbol_vector = &(reinterpret_cast<info_cb_data_t *>(data)->symbols);

	if (function == nullptr) {
		// we didn't get very useful info. fall back to dlsym.
		symbol_vector->emplace_back(backtrace_symbol{
			"",
			0,
			util::symbol_name(reinterpret_cast<void *>(pc), false, true),
			reinterpret_cast<void *>(pc)
		});
	} else {
		symbol_vector->emplace_back(backtrace_symbol{
			(filename == nullptr) ? "" : filename,
			static_cast<unsigned int>(lineno),
			util::demangle(function),
			reinterpret_cast<void *>(pc)
		});
	}

	return 0;
}


void backtrace_pcinfo_error_callback(void *data, const char *msg, int errorno) {
	if (errorno == -1) {
		auto info_cb_data = reinterpret_cast<info_cb_data_t *>(data);

		// no debug info in ELF file.
		// try backtrace_syminfo instead.
		backtrace_syminfo(
			bt_state,
			info_cb_data->pc,
			backtrace_syminfo_callback,
			backtrace_error_callback,
			info_cb_data);
	} else {
		// invoke the general error callback, which prints the message.
		backtrace_error_callback(nullptr, msg, errorno);
	}
}

} // anonymous namespace


void StackAnalyzer::analyze() {
	backtrace_simple(
		bt_state,
		1,                              // skip this many stack frames
		backtrace_simple_callback,
		backtrace_error_callback,
		reinterpret_cast<void *>(this)
	);
}


void StackAnalyzer::get_symbols(std::function<void (const backtrace_symbol *)> cb, bool reversed) const {
	info_cb_data_t info_cb_data;

	for(void *pc : this->stack_addrs) {
		info_cb_data.pc = reinterpret_cast<uintptr_t>(pc);

		// note: a call to backtrace_pcinfo may, in semi-rare cases, push back
		// multiple symbols to result. That's nothing to worry about, though.
		// If you decide you don't like it, make pcinfo_callback return 1.
		backtrace_pcinfo(
			bt_state,
			info_cb_data.pc,
			backtrace_pcinfo_callback,
			backtrace_pcinfo_error_callback,
			reinterpret_cast<void *>(&info_cb_data)
		);
	}

	if (reversed) {
		// this entire vector thing would only be needed for reversed=true...
		for (size_t idx = info_cb_data.symbols.size(); idx-- > 0;) {
			cb(&info_cb_data.symbols[idx]);
		}
	} else {
		for (backtrace_symbol &symbol : info_cb_data.symbols) {
			cb(&symbol);
		}
	}
}

}} // openage::error

#else // WITHOUT_BACKTRACE

// use GNU's <execinfo.h>
#include <execinfo.h>

namespace openage {
namespace error {


void StackAnalyzer::analyze() {
	// unfortunately, backtrace won't tell us how big our buffer
	// needs to be, so we have no choice but to try until it
	// reports success.
	std::vector<void *> buffer{16};
	while (true) {
		int elements = backtrace(buffer.data(), buffer.size());
		if (elements < (ssize_t) buffer.size()) {
			buffer.resize(elements);
			break;
		}
		buffer.resize(buffer.size() * 2);
	}

	for (void *element : buffer) {
		this->stack_addrs.push_back(element);
	}
}


void StackAnalyzer::get_symbols(std::function<void (const backtrace_symbol *)> cb, bool reversed) const {
	backtrace_symbol symbol;
	symbol.filename = "";
	symbol.lineno = 0;

	if (reversed) {
		for (size_t idx = this->stack_addrs.size(); idx-- > 0;) {
			void *pc = this->stack_addrs[idx];

			symbol.functionname = util::symbol_name(pc, false, true);
			symbol.pc = pc;

			cb(&symbol);
		}
	} else {
		for (void *pc : this->stack_addrs) {
			symbol.functionname = util::symbol_name(pc, false, true);
			symbol.pc = pc;

			cb(&symbol);
		}
	}
}


}} // openage::error

#endif // WITHOUT_BACKTRACE


namespace openage {
namespace error {


void StackAnalyzer::trim_to_current_stack_frame() {
	StackAnalyzer current;
	current.analyze();

	while (not current.stack_addrs.empty() and not this->stack_addrs.empty()) {
		if (this->stack_addrs.back() != current.stack_addrs.back()) {
			break;
		}

		this->stack_addrs.pop_back();
		current.stack_addrs.pop_back();
	}
}


}} // openage::error
