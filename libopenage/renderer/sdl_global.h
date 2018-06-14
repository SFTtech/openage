// Copyright 2018-2018 the openage authors. See copying.md for legal info.


namespace openage {
namespace renderer {

/// Lazily initializes the global SDL singleton if it isn't yet available,
/// otherwise does nothing.
void make_sdl_available();

}} // namespace openage::renderer
