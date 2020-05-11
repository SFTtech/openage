// Copyright 2014-2020 the openage authors. See copying.md for legal info.

#pragma once

namespace openage::util {

class ExternalProfiler {
public:
	ExternalProfiler();

	static const char *const profiling_filename;
	static const char *const profiling_pdf_filename;

	bool currently_profiling;
	const bool can_profile;

	void start();
	void stop();

	void show_results();
};

} // namespace openage::util
