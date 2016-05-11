// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#include "externalprofiler.h"

#include <string>

#include "config.h"

#if WITH_GPERFTOOLS_PROFILER
#include <gperftools/profiler.h>
#endif

#include "subprocess.h"
#include "os.h"
#include "../log/log.h"


namespace openage {
namespace util {


ExternalProfiler::ExternalProfiler()
	:
	currently_profiling{false},
	can_profile{WITH_GPERFTOOLS_PROFILER} {}


const char *const ExternalProfiler::profiling_filename = "/tmp/openage-gperftools-cpuprofile";
const char *const ExternalProfiler::profiling_pdf_filename = "/tmp/openage-gperftools-cpuprofile.pdf";


void ExternalProfiler::start() {
	if (not this->can_profile) {
		log::log(MSG(err) << "Can not profile: gperftools is missing");
		return;
	}

	if (this->currently_profiling) {
		log::log(MSG(info) << "Profiler is already running");
		return;
	}

	log::log(MSG(info) << "Starting profiler; writing data to " << this->profiling_filename);

	this->currently_profiling = true;
	#if WITH_GPERFTOOLS_PROFILER
	ProfilerStart(this->profiling_filename);
	#endif
}


void ExternalProfiler::stop() {
	if (not this->can_profile) {
		log::log(MSG(err) << "Can not profile: gperftools is missing");
		return;
	}

	if (not this->currently_profiling) {
		log::log(MSG(err) << "Profiler is not currently running");
		return;
	}

	this->currently_profiling = false;
	#if WITH_GPERFTOOLS_PROFILER
	ProfilerStop();
	#endif

	log::log(MSG(info) << "Profiler stopped; data written to " << this->profiling_filename);
}


void ExternalProfiler::show_results() {
	if (not this->can_profile) {
		log::log(MSG(err) << "Can not profile: gperftools is missing");
		return;
	}

	if (this->currently_profiling) {
		log::log(MSG(warn) << "Profiler is currently running; trying to show results anyway");
	}

	#if WITH_GPERFTOOLS_PROFILER
	std::string pprof_path = subprocess::which("google-pprof");
	// fallback
	if (pprof_path.size() == 0) {
		pprof_path = subprocess::which("pprof");
	}

	if (pprof_path.size() == 0) {
		log::log(MSG(err) << "Can not process profiling results: google-pprof is missing");
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
		log::log(MSG(err) << "Profile analysis failed: " << retval);
		return;
	}

	retval = os::execute_file(this->profiling_pdf_filename);

	if (retval != 0) {
		log::log(MSG(err) <<
			"Could not view profiling visualization " <<
			this->profiling_pdf_filename << ": " << retval);
		return;
	}
	#endif
}

} // namespace util
} // namespace openage
