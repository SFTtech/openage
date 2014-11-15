// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#include "externalprofiler.h"

#include <string>

#include "../config.h"

#if HAS_GPERFTOOLS_PROFILER
#include <gperftools/profiler.h>
#endif

#include "../crossplatform/subprocess.h"
#include "../crossplatform/os.h"
#include "../log.h"

namespace openage {
namespace util {

ExternalProfiler::ExternalProfiler()
	:
	currently_profiling{false},
	can_profile{HAS_GPERFTOOLS_PROFILER} {}

const char *const ExternalProfiler::profiling_filename = "/tmp/openage-gperftools-cpuprofile";
const char *const ExternalProfiler::profiling_pdf_filename = "/tmp/openage-gperftools-cpuprofile.pdf";

void ExternalProfiler::start() {
	if (not this->can_profile) {
		log::err("can not profile: gperftools is missing");
		return;
	}

	if (this->currently_profiling) {
		log::msg("profiler is already running");
		return;
	}

	log::msg("starting profiler; writing data to %s", this->profiling_filename);

	this->currently_profiling = true;
	#if HAS_GPERFTOOLS_PROFILER
	ProfilerStart(this->profiling_filename);
	#endif
}

void ExternalProfiler::stop() {
	if (not this->can_profile) {
		log::err("can not profile: gperftools is missing");
		return;
	}

	if (not this->currently_profiling) {
		log::msg("profiler is not currently running");
		return;
	}

	this->currently_profiling = false;
	#if HAS_GPERFTOOLS_PROFILER
	ProfilerStop();
	#endif

	log::msg("profiler stopped; data written to %s", this->profiling_filename);
}

void ExternalProfiler::show_results() {
	if (not this->can_profile) {
		log::err("can not profile: gperftools is missing");
		return;
	}

	if (this->currently_profiling) {
		log::warn("profiler is currently running; trying to show results anyway");
	}

	#if HAS_GPERFTOOLS_PROFILER
	std::string pprof_path = subprocess::which("google-pprof");
	// fallback
	if (pprof_path.size() == 0) {
		pprof_path = subprocess::which("pprof");
	}

	if (pprof_path.size() == 0) {
		log::err("can not process profiling results: google-pprof is missing");
		return;
	}

	int retval = subprocess::call(
		{
			pprof_path.c_str(),
			"--pdf",
			"--",
			os::self_exec_filename().c_str(),
			this->profiling_filename,
			nullptr
		},
		true,
		this->profiling_pdf_filename);

	if (retval != 0) {
		log::err("profile analysis failed: %d", retval);
		return;
	}

	retval = os::execute_file(this->profiling_pdf_filename);

	if (retval != 0) {
		log::err("could not view profiling visualization %s: %d", this->profiling_pdf_filename, retval);
		return;
	}
	#endif
}

} // namespace util
} // namespace openage
