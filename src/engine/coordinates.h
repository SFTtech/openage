#ifndef _ENGINE_COORDINATES_H_
#define _ENGINE_COORDINATES_H_

#include <stdint.h>

#include "vector2.h"
#include "vector3.h"

//this header file is in accordance to doc:implementation/coordinate-systems

namespace openage {
namespace engine {
namespace coord {
	//units:       1px
	//resolution:  1px
	//value range: [-32768 px, +32767 px]
	using sdl_t = int16_t;
	using sdl = vector2<sdl_t, true>;
	using sdl_delta = vector2<sdl_t, false>;

	//units:       1/2^16 tu/teu
	//resolution:  1.5e-5 tu/teu
	//value range: [-1.4e13 tu/teu, +1.4e13 tu/teu]
	using phys_t = int64_t;
	using phys = vector3<phys_t, true>;
	using phys_delta = vector3<phys_t, false>;

	//units:       1px
	using hud_t = float;
	using hud = vector2<hud_t, true>;
	using hud_delta = vector2<hud_t, false>;

	//units:       1px
	using camera_t = float;
	using camera = vector2<camera_t, true>;
	using camera_delta = vector2<camera_t, false>;

	sdl phys_to_sdl(phys in);
	sdl hud_to_sdl(hud in);
	sdl camera_to_sdl(camera in);

	sdl_delta phys_to_sdl(phys_delta in);
	sdl_delta hud_to_sdl(hud_delta in);
	sdl_delta camera_to_sdl(camera_delta in);

	phys sdl_to_phys(sdl in);
	phys hud_to_phys(hud in);
	phys camera_to_phys(camera in);

	phys_delta sdl_to_phys(sdl_delta in);
	phys_delta hud_to_phys(hud_delta in);
	phys_delta camera_to_phys(camera_delta in);

	hud sdl_to_hud(sdl in);
	hud phys_to_hud(phys in);
	hud camera_to_hud(camera in);

	hud_delta sdl_to_hud(sdl_delta in);
	hud_delta phus_to_hud(phys_delta in);
	hud_delta camera_to_hud(camera_delta in);

	camera sdl_to_camera(sdl in);
	camera phys_to_camera(phys in);
	camera hud_to_camera(hud in);

	camera_delta sdl_to_camera(sdl_delta in);
	camera_delta phys_to_camera(phys_delta in);
	camera_delta hud_to_camera(hud_delta in);

} //namespace coord
} //namespace engine
} //namespace openage

#endif //_ENGINE_COORDINATES_H_
