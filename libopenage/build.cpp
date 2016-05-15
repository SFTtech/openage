// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#include "build.h"

#include <libopenage/config.h>


namespace openage {


build_configuration::build_configuration()
	:
	global_asset_dir{config::global_asset_dir},
	config_options{config::config_option_string},
	build_src_dir{config::buildsystem_sourcefile_dir},
	compiler{config::compiler},
	compiler_flags{config::compiler_flags},
	cython_version{config::cython_version} {

}


} // namespace openage
