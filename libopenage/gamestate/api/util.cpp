// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "util.h"

#include <regex>
#include <vector>

#include <nyan/nyan.h>

#include "error/error.h"
#include "log/message.h"


namespace openage::gamestate::api {

const std::string resolve_file_path(const nyan::Object &obj, const std::string &path) {
	if (path.empty()) {
		return path;
	}

	if (path[0] == '/') {
		// absolute path
		std::regex modpack_id_regex("{[A-Za-z0-9_]+@[A-Za-z0-9_]+}");
		std::smatch match;
		if (std::regex_match(path, match, modpack_id_regex)) {
			auto modpack_id = match[0].str().substr(1, match[0].str().size() - 2);

			// TODO: get mount point of modpack

			auto path_end = match.suffix().str();

			return modpack_id + path_end;
		}
		else {
			throw Error(MSG(err) << "Absolute file path '" << path << "' does not contain a modpack ID.");
		}
	}
	else {
		// relative path
		std::string obj_path = obj.get_info().get_namespace().to_dirpath();
		return obj_path + "/" + path;
	}
}

} // namespace openage::gamestate::api
