#ifndef _ENGINE_COORDINATES_H_
#define _ENGINE_COORDINATES_H_

#include <stdint.h>

//this header file is in accordance to doc:implementation/coordinate-systems

namespace openage {
	struct coord_sdl {
		uint16_t x; //between 0 px and 65535 px
		uint16_t y; //between 0 px and 65535 px
	};

	struct coord_phys {
		//the lower 16 bits of all members are fixed point places,
		//leading to an accuracy of 1.5e-5 tu/thu

		int64_t ne; //between -1.4e13 tu and 1.4e13 tu
		int64_t se; //between -1.4e13 tu and 1.4e13 tu
		int32_t up; //between -3.2e4 thu and 1.4e4 thu
	};

	struct coord_hud {
		float x;
		float y;
	};

	struct coord_camera {
		float x;
		float y;
	};
}

} //namespace openage

#endif //_ENGINE_COORDINATES_H_
