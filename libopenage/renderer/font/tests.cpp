// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "../../testing/testing.h"

#include "font_manager.h"
#include "font.h"

namespace openage {
namespace renderer {
namespace tests {

void font_manager_test_get_font() {
	FontManager font_manager;

	// FontManager should provide different font instances for different sizes of same font face.
	Font *font1 = font_manager.get_font("DejaVu Serif", "Book", 12);
	Font *font2 = font_manager.get_font("DejaVu Serif", "Book", 20);
	(font1 != font2) or TESTFAIL;

	// FontManager should provide the cached font instance
	Font *font3 = font_manager.get_font("DejaVu Serif", "Book", 12);
	(font3 == font1) or TESTFAIL;
}

void font_manager() {
	font_manager_test_get_font();
}

void font_test_font_description() {
	// Test default direction, language, script of font_description
	font_description fd1{"DejaVu Serif", "Book", 12};
	(fd1.direction == font_direction::left_to_right) or TESTFAIL;
	(fd1.language == "en") or TESTFAIL;
	(fd1.script == "Latn") or TESTFAIL;

	// Equality check of two different font_description instances
	font_description fd2{"DejaVu Serif", "Book", 20};
	(fd1 != fd2) or TESTFAIL;

	// a font_description instance constructed from family,style,size should be equal to another with same font file.
	font_description fd3{fd1.font_file, 12};
	(fd3 == fd1) or TESTFAIL;

	// Equality check with copy constructor and assignment operator
	font_description fd4 = fd3;
	font_description fd5{fd4};
	(fd4 == fd3) or TESTFAIL;
	(fd5 == fd3) or TESTFAIL;

	// Equality check with different font direction, language, script
	fd3.direction = font_direction::right_to_left;
	fd4.language = "fr";
	fd5.script = "Deva";
	(fd3 != fd1) or TESTFAIL;
	(fd4 != fd1) or TESTFAIL;
	(fd5 != fd1) or TESTFAIL;
}

void font() {
	font_test_font_description();
}

}}} // openage::renderer::tests
