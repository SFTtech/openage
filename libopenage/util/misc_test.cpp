// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "misc.h"

#include "../testing/testing.h"


namespace openage {
namespace util {
namespace tests {


void array_conversion() {
	{
		static const std::vector<uint8_t> test_array8_big{
			0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
		};
		static const std::vector<uint8_t> test_array8_little{
			0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01
		};
		static const uint64_t test_uint64{0x0123456789abcdef};

		// Big-endian conversions
		array8_to_uint64(test_array8_big.data(), 8, true) == test_uint64 or TESTFAIL;
		uint64_to_array8(test_uint64, true) == test_array8_big or TESTFAIL;
		// Little-endian conversions
		array8_to_uint64(test_array8_little.data(), 8, false) == test_uint64 or TESTFAIL;
		uint64_to_array8(test_uint64, false) == test_array8_little or TESTFAIL;
	}

	{
		static const std::vector<uint8_t> test_array8_big{
			0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
			0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7
		};
		static const std::vector<uint8_t> test_array8_little{
			0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01,
			0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0
		};
		static const std::vector<uint64_t> test_array64{
			0x0123456789abcdef, 0xf0f1f2f3f4f5f6f7
		};
		// Big-endian conversions
		array8_to_array64(test_array8_big.data(), 16, true) == test_array64 or TESTFAIL;
		array64_to_array8(test_array64.data(), 2, true) == test_array8_big or TESTFAIL;
		// Little-endian conversions
		array8_to_array64(test_array8_little.data(), 16, false) == test_array64 or TESTFAIL;
		array64_to_array8(test_array64.data(), 2, false) == test_array8_little or TESTFAIL;
	}

	// Now I'm converting 14 instead of 16 bytes, when using array8_to_array64 the
	// remaining bytes should be filled with zeroes.
	{
		static const std::vector<uint8_t> test_array8_big{
			0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
			0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0x00, 0x00
		};
		static const std::vector<uint64_t> test_array64_big{
			0x0123456789abcdef, 0xf0f1f2f3f4f50000
		};

		static const std::vector<uint8_t> test_array8_little{
			0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
			0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0x00, 0x00
		};
		static const std::vector<uint64_t> test_array64_little{
			0xefcdab8967452301, 0x0000f2f3f4f5f6f7
		};

		// Big-endian conversions
		array8_to_array64(test_array8_big.data(), 14, true) == test_array64_big or TESTFAIL;
		array64_to_array8(test_array64_big.data(), 2, true) == test_array8_big or TESTFAIL;

		// Little-endian conversions
		array8_to_array64(test_array8_little.data(), 14, false) == test_array64_little or TESTFAIL;
		array64_to_array8(test_array64_little.data(), 2, false) == test_array8_little or TESTFAIL;
	}
}

}}} // openage::util::tests
