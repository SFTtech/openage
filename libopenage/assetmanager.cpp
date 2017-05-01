// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include "assetmanager.h"

#include <map>
#include <SDL2/SDL_image.h>

#if WITH_INOTIFY
#include <sys/inotify.h>
#include <unistd.h>
#include <limits.h> /* for NAME_MAX */
#endif

#include "util/compiler.h"
#include "util/file.h"
#include "error/error.h"
#include "log/log.h"

#include "texture.h"
#include "datastructure/task_queue.h"

#include "job/job_manager.h"

namespace openage {

AssetManager::AssetManager(qtsdl::GuiItemLink *gui_link)
	:
	missing_tex{nullptr},
	gui_link{gui_link} {

#if WITH_INOTIFY
	// initialize the inotify instance
	this->inotify_fd = inotify_init1(IN_NONBLOCK);
	if (this->inotify_fd < 0) {
		throw Error{MSG(err) << "Failed to initialize inotify!"};
	}
#endif
}


const util::Path &AssetManager::get_asset_dir() {
	return this->asset_path;
}


void AssetManager::set_asset_dir(const util::Path& new_path) {
	if (this->asset_path != new_path) {
		this->asset_path = new_path;
		this->clear();
	}
}


void AssetManager::set_engine(Engine *engine) {
	this->engine = engine;
}


Engine *AssetManager::get_engine() const {
	return this->engine;
}

std::shared_ptr<Texture> AssetManager::load_texture(const std::string &name,
                                                    bool use_metafile,
                                                    bool null_if_missing,
                                                    SDL_Surface *surface) {
	// the texture to be associated with the given filename
	std::shared_ptr<Texture> tex;

	util::Path tex_path = this->asset_path[name];

	// try to open the texture filename.
	if (not tex_path.is_file()) {
		// TODO: add/fetch inotify watch on the containing folder
		// to display the tex as soon at it exists.

		if (null_if_missing) {
			return nullptr;
		}
		else {
			// return the big X texture instead
			tex = this->get_missing_tex();
		}
	}
	else {
		// create the texture!
		tex = std::make_shared<Texture>(tex_path, use_metafile, surface);

#if WITH_INOTIFY
		std::string native_path = tex_path.resolve_native_path();

		if (native_path.size() > 0) {
			// create inotify update trigger for the requested file

			// TODO: let util::Path do the file watching
			int wd = inotify_add_watch(
				this->inotify_fd,
				native_path.c_str(),
				IN_CLOSE_WRITE
			);

			if (wd < 0) {
				log::log(WARN << "Failed to add inotify watch for " << native_path);
			} else {
				this->watch_fds[wd] = tex;
			}
		}
#endif
	}

	// pass back the shared_ptr<Texture>
	return tex;
}

void AssetManager::load_textures(const std::vector<std::string> &names,
                                 bool use_metafile,
                                 bool null_if_missing) {
	using namespace job;
	using ret_pair_t = std::pair<std::string, SDL_Surface *>;
	class JobFunc {
		std::string name;

	 public:
		JobFunc(const std::string &name) : name(name) {}
		ret_pair_t operator()() const {
			return std::make_pair(name, IMG_Load(name.c_str()));
		}
	};
	JobManager jobManager(4);
	jobManager.start();

	std::atomic<int> executed_callbacks{0};

	std::map<std::string, SDL_Surface *> path_to_surface;
	std::mutex path_to_surface_lock;
	auto job_callback =
	    [&path_to_surface, &executed_callbacks,
	     &path_to_surface_lock](result_function_t<ret_pair_t> get_result) {
		    ret_pair_t job_result = get_result();
		    {
			    std::lock_guard<std::mutex> lock(path_to_surface_lock);
			    path_to_surface.insert(job_result);
		    }
		    ++executed_callbacks;
		  };

	std::vector<std::string> validNames;
	for (const auto &fname : names) {
		const util::Path tex_path = this->asset_path[fname];
		if (tex_path.is_file()) {
			std::string native_path = tex_path.resolve_native_path();
			jobManager.enqueue<ret_pair_t>(JobFunc(native_path), job_callback);
			validNames.push_back(fname);
		}
	}

	while (executed_callbacks < validNames.size()) {
		jobManager.execute_callbacks();
	}

	for (const auto &fname : validNames) {
		const util::Path tex_path = this->asset_path[fname];
		const std::string native_path = tex_path.resolve_native_path();
		SDL_Surface *surface = path_to_surface[native_path];
		get_texture(fname, use_metafile, null_if_missing, surface);
	}
}

Texture *AssetManager::get_texture(const std::string &name,
                                   bool use_metafile,
                                   bool null_if_missing,
                                   SDL_Surface *surface) {
	// check whether the requested texture was loaded already
	auto tex_it = this->textures.find(name);

	// the texture was not loaded yet:
	if (tex_it == this->textures.end()) {
		auto tex = this->load_texture(name, use_metafile, null_if_missing,surface);

		if (tex.get() != nullptr) {
			// insert the texture into the map
			this->textures.insert(std::make_pair(name, tex));
		}

		//  and return the texture pointer.
		return tex.get();
	}

	return tex_it->second.get();
}


void AssetManager::check_updates() {
#if WITH_INOTIFY
	// buffer for at least 4 inotify events
	char buf[4 * (sizeof(struct inotify_event) + NAME_MAX + 1)];
	ssize_t len;

	while (true) {
		// fetch all events, the kernel won't write "half" structs.
		len = read(this->inotify_fd, buf, sizeof(buf));

		if (len == -1) {
			if (errno == EAGAIN) {
				// no events, nothing to do.
				break;
			} else {
				// something went wrong
				log::log(WARN << "Failed to read inotify events!");
				break;
			}
		}

		// process fetched events,
		// the kernel guarantees complete events in the buffer.
		char *ptr = buf;
		while (ptr < buf + len) {
			struct inotify_event *event = (struct inotify_event *)ptr;

			if (event->mask & IN_CLOSE_WRITE) {
				// TODO: this should invoke callback functions
				this->watch_fds[event->wd]->reload();
			}

			// move the buffer ptr to the next event.
			ptr += sizeof(struct inotify_event) + event->len;
		}
	}
#endif
}

std::shared_ptr<Texture> AssetManager::get_missing_tex() {

	// if not loaded, fetch the "missing" texture (big red X).
	if (unlikely(this->missing_tex.get() == nullptr)) {
		this->missing_tex = std::make_shared<Texture>(
			this->asset_path["missing.png"],
			false
		);
	}

	return this->missing_tex;
}

void AssetManager::clear() {
#if WITH_INOTIFY
	for (auto& watch_fd : this->watch_fds) {
		int result = inotify_rm_watch(this->inotify_fd, watch_fd.first);
		if (result < 0) {
			log::log(WARN << "Failed to remove inotify watches");
		}
	}
	this->watch_fds.clear();
#endif

	this->textures.clear();
}

} // openage
