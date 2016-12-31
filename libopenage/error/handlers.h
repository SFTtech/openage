// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp cimport bool as cppbool

namespace openage {
namespace error {


/**
 * Call this to set or unset the atexit error handler.
 *
 * In order to capture any stray calls to exit() from any
 * place in the game (including libraries), set this to true
 * on startup, and back to false right before termination.
 *
 * pxd: void set_exit_ok(cppbool value) except +
 */
void set_exit_ok(bool value);


}} // openage::error
