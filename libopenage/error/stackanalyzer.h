// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_ERROR_STACKANALYZER_H_
#define OPENAGE_ERROR_STACKANALYZER_H_

#include "backtrace.h"

namespace openage {
namespace error {

/**
 * Implementation of the Backtrace interface that analyzes the current C++
 * stack.
 *
 * The Usage is:
 *
 * StackAnalyzer sa;
 * sa.analyze();
 *
 * Integrating analyze() into the constructor would be a bad idea because then,
 * all sorts of allocators and other gory constructor internals might be
 * visible in the Backtrace.
 *
 * The implementation  of analyze() and get_symbols() may use all sorts of
 * analyzers, depending on what's available on the platform.
 * The quality of the resolved symbol names may vary accordingly.
 */
class StackAnalyzer : public Backtrace {
public:
	/**
	 * Creates an empty StackAnalyzer object.
	 */
	StackAnalyzer() = default;

	/*
	 * Analyzes the current stack, and stores the program counter values in
	 * this->stack_addrs.
	 */
	void analyze();

	/**
	 * All program counters of this backtrace.
	 */
	std::vector<void *> stack_addrs;

	// Looks up symbol names for the program counter values.
	void get_symbols(std::function<void (const backtrace_symbol *)> cb, bool reversed) const override;

	void trim_to_current_stack_frame() override;
};

}} // openage::error

#endif
