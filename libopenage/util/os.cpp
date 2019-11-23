// Copyright 2014-2019 the openage authors. See copying.md for legal info.

#include "os.h"

#include <memory>

#ifdef _WIN32
// TODO not yet implemented
#else
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#ifdef __FreeBSD__
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#include "../log/log.h"
#include "subprocess.h"

namespace openage::os {

std::string read_symlink(const char *path) {
#ifdef _WIN32
	// TODO not yet implemented
	return "";
#else
	size_t bufsize = 1024;

	while (true) {
		bufsize *= 2;
		std::unique_ptr<char[]> buf{new char[bufsize]};

		ssize_t len = readlink(path, buf.get(), bufsize);

		if (len < 0) {
			log::log(MSG(err) << "could not read link " << path);
			return "";
		}

		if (static_cast<size_t>(len) >= bufsize) {
			continue;
		}

		buf[len] = '\0';

		return std::string{buf.get()};
	}
#endif
}

std::string self_exec_filename() {
	// nice overview of crossplatform methods:
	// http://stackoverflow.com/questions/5919996

#ifdef __linux
	return read_symlink("/proc/self/exe");
#elif __APPLE__
	uint32_t bufsize = 1024;

	while (true) {
		std::unique_ptr<char[]> buf{new char[bufsize]};

		if (_NSGetExecutablePath(buf.get(), &bufsize) < 0) {
			continue;
		}

		return std::string{buf.get()};
	}
#elif __FreeBSD__
	size_t bufsize = 1024;
	int mib[4] = {
		CTL_KERN,
		KERN_PROC,
		KERN_PROC_PATHNAME,
		-1
	};

	while (true) {
		std::unique_ptr<char[]> buf{new char[bufsize]};

		if (sysctl(mib, 4, buf.get(), &bufsize, nullptr, 0) < 0) {
			continue;
		}

		return std::string{buf.get()};
	}

#elif _WIN32
	// TODO not yet implemented
	return std::string("openage.exe"); // FIXME: wild guess though
#else
	static_assert(false, "subprocess::self_filename is not yet implemented for... whatever platform you're using right now.");
#endif
}

int execute_file(const char *path, bool background) {
#ifdef _WIN32
	// TODO some sort of shell-open, not yet implemented
	return -1; // failure
#else
		std::string runner = "";
#ifdef __APPLE__
			runner = subprocess::which("open");
#else
			runner = subprocess::which("xdg-open");
			// fallback
			if (runner.size() == 0) {
				runner = subprocess::which("gnome-open");
			}
#endif
		if (runner.size() == 0) {
			log::log(MSG(err) << "could not locate file-opener");
			return -1;
		}

		return subprocess::call({runner.c_str(), path, nullptr}, not background);
#endif
}

} // namespace openage
