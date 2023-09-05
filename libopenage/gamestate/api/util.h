// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

#include <nyan/nyan.h>


namespace openage::gamestate::api {

/**
 * Get the file path from a reference in \p file member of a nyan object.
 *
 * File references in \p file members can be relative (to the nyan object)
 * or absolute (relative to a modpack mount folder). See
 * https://github.com/SFTtech/openage/blob/master/doc/media/openage/file_referencing.md
 * for more info.
 *
 * @param obj nyan object where the file reference is stored.
 * @param path File reference (relative or absolute).
 *
 * @return Relative file path from the modpack root folder.
 */
const std::string resolve_file_path(const nyan::Object &obj, const std::string &path);

} // namespace openage::gamestate::api
