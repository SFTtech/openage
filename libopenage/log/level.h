// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libopenage.util.enum cimport Enum, EnumValue
#include "../util/enum.h"


namespace openage {
namespace log {

/**
 * Data associated with a log level.
 *
 * pxd:
 *
 * cppclass level_value(EnumValue[level_value, int]):
 *     const char *colorcode
 */
struct OAAPI level_value : util::EnumValue<level_value> {
	const char *colorcode;
};


/**
 * Available logging levels.
 *
 * pxd:
 *
 * cppclass level(Enum[level_value]):
 *     pass
 *
 * level MIN  "::openage::log::level::MIN"
 * level spam "::openage::log::level::spam"
 * level dbg  "::openage::log::level::dbg"
 * level info "::openage::log::level::info"
 * level warn "::openage::log::level::warn"
 * level err  "::openage::log::level::err"
 * level crit "::openage::log::level::crit"
 * level MAX  "::openage::log::level::MAX"
 */

struct OAAPI level : util::Enum<level_value> {
	using util::Enum<level_value>::Enum;

	// a default constructor for level.
	// this is needed to allow use from Cython.
	// initializes the level to an internal UNDEFINED value.
	level();

	static constexpr level_value MIN       {{"min loglevel", -1000}, "5"};

	static constexpr level_value spam      {{"SPAM",          -100}, ""};
	static constexpr level_value dbg       {{"DBG",            -20}, ""};
	static constexpr level_value info      {{"INFO",             0}, ""};
	static constexpr level_value warn      {{"WARN",           100}, "33"};
	static constexpr level_value err       {{"ERR",            200}, "31;1"};
	static constexpr level_value crit      {{"CRIT",           500}, "31;1;47"};

	static constexpr level_value MAX       {{"max loglevel",  1000}, "5"};
};

}} // namespace openage::log
