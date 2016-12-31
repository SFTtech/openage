# Copyright 2015-2015 the openage authors. See copying.md for legal info.
#
# (To distribute this file outside of openage, extend the above
#  copyright line with an appropriate GPLv3 or later reference,
#  as you probably don't have a our copying.md)


#=================================================================
# Locate Vulkan graphics library
#
# usage:
#   find_package(Vulkan)
#
# This module sets the following variables:
# VULKAN_FOUND            True, if the system has Vulkan.
# VULKAN_INCLUDE_DIR      Path to the Vulkan include directory.
# VULKAN_LIBRARIES        Paths to the Vulkan libraries.
#=================================================================

set(_Vulkan_REQUIRED_VARS "VULKAN_vk_LIBRARY" "VULKAN_INCLUDE_DIR")

find_path(VULKAN_INCLUDE_DIR
	VK/vk.h
	/opt/graphics/Vulkan/include
)

find_library(VULKAN_vk_LIBRARY
	NAMES VK Vulkan
	PATHS
	/opt/graphics/Vulkan/lib
)

if(VULKAN_vk_LIBRARY)
	set(VULKAN_LIBRARIES ${VULKAN_vk_LIBRARY})
endif()


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
	Vulkan
	REQUIRED_VARS ${_Vulkan_REQUIRED_VARS}
	FAIL_MESSAGE "Vulkan NOT found."
)

unset(_Vulkan_REQUIRED_VARS)

mark_as_advanced(
	VULKAN_INCLUDE_DIR
	VULKAN_vk_LIBRARY
)
