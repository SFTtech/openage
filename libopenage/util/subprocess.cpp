// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#include "subprocess.h"

#include <cstdlib>
#include <cstring>
#include <cerrno>

#ifdef _WIN32
// TODO not yet implemented
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#endif

#include "../log/log.h"
#include "strings.h"

namespace openage {
namespace subprocess {

bool is_executable(const char *filename) {
	#ifdef _WIN32
	// TODO not yet implemented
	// static_assert(false, "subprocess::is_executable is not yet implemented for WIN32");
	return false;
	#else
	struct stat sb;
	return (stat(filename, &sb) == 0
	        and S_ISREG(sb.st_mode)
	        and sb.st_mode & 0111);
	#endif
}

std::string which(const char *name) {
	#ifdef _WIN32
	// TODO not yet implemented
	// static_assert(false, "subprocess::which is not yet implemented for WIN32");
	return std::string(name);
	#else
	std::vector<char> path;
	util::copy_string(getenv("PATH"), path);

	if (is_executable(name)) {
		return name;
	}

	for (char *dir = strtok(path.data(), ":"); dir; dir = strtok(NULL, ":")) {
		std::string filename;
		filename.append(dir);
		filename.push_back('/');
		filename.append(name);

		if (is_executable(filename.c_str())) {
			return filename;
		}
	}

	return "";
	#endif
}

int call(const std::vector<const char *> &argv, bool wait, const char *redirect_stdout_to) {
	#ifdef _WIN32
	// TODO not yet implemented
	// static_assert(false, "subprocess::call is not yet implemented for WIN32");
	return -1; // nope
	#else

	// used by child to communicate execve() to its parent.
	// on success, the child auto-closes the pipe; the parent reads 0 bytes.
	// on failure, however, the child writes errno; the parent reads this.
	int pipefd[2];

	if (pipe(pipefd) < 0) {
		// the pipe could not be created
		log::log(MSG(err) << "could not create pipe: " << strerror(errno));

		return -1;
	}

	int replacement_stdout_fd = -1;
	if (redirect_stdout_to != nullptr) {
		replacement_stdout_fd = open(
			redirect_stdout_to,
			O_WRONLY | O_CREAT|O_TRUNC|O_CLOEXEC,
			0644);

		if (replacement_stdout_fd < 0) {
			log::log(MSG(err) << "could not open output redirection file " << redirect_stdout_to << ": " << strerror(errno));

			close(pipefd[0]);
			close(pipefd[1]);

			return -1;
		}
	}

	// mark write end of pipe as close-on-exec
	if (fcntl(pipefd[1], F_SETFD, FD_CLOEXEC) < 0) {
		log::log(MSG(err) << "could not fcntl write-end of pipe: " << strerror(errno));

		close(pipefd[0]);
		close(pipefd[1]);

		if (replacement_stdout_fd > -1) {
			close(replacement_stdout_fd);
		}

		return -1;
	}

	pid_t child_pid = fork();

	if (child_pid == -1) {
		// the fork has failed
		log::log(MSG(err) << "could not fork: " << strerror(errno));

		close(pipefd[0]);
		close(pipefd[1]);

		if (replacement_stdout_fd > -1) {
			close(replacement_stdout_fd);
		}

		return -1;
	}

	if (child_pid == 0) {
		// we're the child

		// close read end of pipe
		close(pipefd[0]);

		// replace stdout
		if (replacement_stdout_fd > -1) {
			dup2(replacement_stdout_fd, 1);
			close(replacement_stdout_fd);
		}

		const char *const *argv_data = argv.data();

		// this is horrible.
		// "(...) these objects are completely constant."
		// "Due to a limitation of the ISO C standard,"
		// "it is not possible to state that idea in standard C."
		// wtf is your problem, guys...
		execv(argv[0], const_cast<char *const *>(argv_data));

		int child_errno = errno;

		// execv has failed. write errno to parent

		// welcome to our little adventure of writing _4_ bytes to our parent :)
		char *write_buf = (char *) &child_errno;
		ssize_t remaining = sizeof(child_errno);

		while(remaining > 0) {
			ssize_t written = write(pipefd[1], write_buf, remaining);

			if (written <= 0) {
				// we didn't even succeed at passing errno to our parent.
				// truly, all hope is lost.
				exit(2);
			}

			remaining -= written;
			write_buf += written;
		}
		exit(1);
	}

	// we're the parent.
	close(pipefd[1]);
	if (replacement_stdout_fd > -1) {
		close(replacement_stdout_fd);
	}

	// welcome to our little adventure of reading _4_ bytes from our child :)
	int child_errno;
	char *read_buf = (char *) &child_errno;
	size_t total = 0;

	// we expect to read either 0, or sizeof(child_errno) bytes.
	while (total < sizeof(child_errno)) {
		ssize_t read_count = read(pipefd[0], read_buf + total, sizeof(child_errno) - total);

		if (read_count == 0) {
			// no more data from child
			break;
		}

		if (read_count < 0) {
			log::log(MSG(err) << "read from child pipe failed" << strerror(errno));
			close(pipefd[0]);
			return -1;
		}

		total += read_count;
	}

	close(pipefd[0]);

	if (total > 0) {
		if (total != sizeof(int)) {
			log::log(MSG(err) << "wrong number of bytes read from child pipe: " << total);
			return -1;
		}

		if (child_errno > 0) {
		    log::log(MSG(err) << "execv has failed: " << strerror(child_errno));
		    return -1;
		}
	}

	if (!wait) {
		// leave the child process to its fate
		// TODO wait in thread to avoid zombification?
		return 0;
	}

	// wait for the child process to finish
	int status;
	if (waitpid(child_pid, &status, 0) < 0) {
		log::log(MSG(err) << "could not wait for child process");
		return -1;
	}

	// everything went well.
	return status;
	#endif
}

} // namespace subprocess
} // namespace openage
