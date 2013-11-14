#include "coordinates.h"

#include "engine.h"
#include "terrain.h"

//note that the overloaded operators in this file rely heavily on compiler
//optimizations.
//a non-optimizing compiler would often need to generate several nested method calls,
//while an LTO-optimizing compiler might simply inline two multiplications or casts.
namespace openage {
namespace engine {
namespace coord {

sdl phys_to_sdl(phys in) {
	return camera_to_sdl(phys_to_camera(in));
}

sdl hud_to_sdl(hud in) {
	return sdl {(sdl_t) in.x, (sdl_t) in.y};
}

sdl camera_to_sdl(camera in) {
	return sdl {(sdl_t) in.x, (sdl_t) in.y} + rel(camera_pos_sdl);
}

sdl_delta phys_to_sdl(phys_delta in) {
	return camera_to_sdl(phys_to_camera(in));
}

sdl_delta hud_to_sdl(hud_delta in) {
	return sdl_delta {(sdl_t) in.x, (sdl_t) in.y};
}

sdl_delta camera_to_sdl(camera_delta in) {
	return sdl_delta {(sdl_t) in.x, (sdl_t) in.y};
}

phys sdl_to_phys(sdl in) {
	return camera_to_phys(sdl_to_camera(in));
}

phys hud_to_phys(hud in) {
	return camera_to_phys(hud_to_camera(in));
}

phys camera_to_phys(camera in) {
	//apply scaling factor to 'in', to get 'scaled'
	//scaling factor: w/2 for x, h/2 for y,
	//and the (1 << 16) fixed-point scaling factor for both.
	camera scaled;
	scaled.x = in.x * ((float) (1 << 16)) / tile_halfsize.x;
	scaled.y = in.y * ((float) (1 << 16)) / tile_halfsize.y;

	//apply transformation matrix to 'scaled', to get the position
	//in coord::phys, relative to camera_pos_phys.
	//use the reverse of the phys-to-camera matrix.
	//
	//the equations can not be solved without additional information,
	//because the 2-dimensional input information is not sufficient to calculate
	//3-dimensional output information.
	//the input information rather represents a line in the 3D physics space,
	//which must be intersected with an physics object to determine an actual
	//point.
	//for now, we just assume that result.up == 0,
	//i.e. relative_phys.up == -camera_pos_phys.up.
	//
	//TODO when implementing the terrain elevation milestone,
	//instead find an intersection between the mouse line and terrain grid.
	//
	// the resulting matrix for any given value of up is:
	//
	// (ne) = ( 0.5 -0.5) (scaled_pos.x     )
	// (se) = ( 0.5  0.5) (scaled_pos.y + up)
	phys_delta relative_phys;
	relative_phys.up = -camera_pos_phys.up;
	relative_phys.ne = (scaled.x - scaled.y - relative_phys.up)/2;
	relative_phys.se = (scaled.x + scaled.y + relative_phys.up)/2;

	//add camera camera_pos_phys to relative_phys, yielding an absolute position
	//in coord::phys
	return camera_pos_phys + relative_phys;
}

phys_delta sdl_to_phys(sdl_delta in) {
	return camera_to_phys(sdl_to_camera(in));
}

phys_delta hud_to_phys(hud_delta in) {
	return camera_to_phys(hud_to_camera(in));
}

phys_delta camera_to_phys(camera_delta in) {
	//apply scaling factor to 'in', yielding 'scaled'
	//scaling factor: w/2 for x, h/2 for y,
	//and the (1 << 16) fixed-point scaling factor for both.
	camera_delta scaled;
	scaled.x = in.x * ((float) (1 << 16)) / tile_halfsize.x;
	scaled.y = in.y * ((float) (1 << 16)) / tile_halfsize.y;

	//apply transformation matrix to 'scaled', yielding 'result'
	//use the reverse of the phys-to-camera matrix.
	//
	//again, additional information is required to solve the equations.
	//we assume delta-up == 0.
	//
	// the resulting matrix is:
	//
	// (ne) = ( 0.5 -0.5) (scaled.x)
	// (se) = ( 0.5  0.5) (scaled.y)
	phys_delta result;
	result.up = 0;
	result.ne = (scaled.x - scaled.y) / 2;
	result.se = (scaled.x + scaled.y) / 2;

	return result;
}

hud sdl_to_hud(sdl in) {
	return hud {(hud_t) in.x, (hud_t) in.y};
}

hud phys_to_hud(phys in) {
	return camera_to_hud(phys_to_camera(in));
}

hud camera_to_hud(camera in) {
	return hud {(hud_t) in.x, (hud_t) in.y} + sdl_to_hud(rel(camera_pos_sdl));
}

hud_delta sdl_to_hud(sdl_delta in) {
	return hud_delta {(hud_t) in.x, (hud_t) in.y};
}

hud_delta phys_to_hud(phys_delta in) {
	return camera_to_hud(phys_to_camera(in));
}

hud_delta camera_to_hud(camera_delta in) {
	return hud_delta {(hud_t) in.x, (hud_t) in.y};
}

camera sdl_to_camera(sdl in) {
	in -= rel(camera_pos_sdl);
	return camera{(camera_t) in.x, (camera_t) in.y};
}

camera phys_to_camera(phys in) {
	//subtract camera position, to get relative_phys
	phys_delta relative_phys = in - camera_pos_phys;

	//apply transformation matrix to relative_phys, to get 'scaled':
	//                  (ne)
	// (x) = ( 1  1  0) (se)
	// (y) = (-1  1 -1) (up)
	camera scaled;
	scaled.x = +1 * relative_phys.ne +1 * relative_phys.se +0 * relative_phys.up;
	scaled.y = -1 * relative_phys.ne +1 * relative_phys.se -1 * relative_phys.up;

	//remove scaling factor from scaled, to get result
	//scaling factor: w/2 for x, h/2 for y
	//and the (1 << 16) fixed-point scaling factor for both.
	camera result;
	result.x = scaled.x * tile_halfsize.x / ((float) (1 << 16));
	result.y = scaled.y * tile_halfsize.y / ((float) (1 << 16));

	return result;
}

camera hud_to_camera(hud in) {
	in -= sdl_to_hud(rel(camera_pos_sdl));
	return camera {(camera_t) in.x, (camera_t) in.y};
}

camera_delta sdl_to_camera(sdl_delta in) {
	return camera_delta {(camera_t) in.x, (camera_t) in.y};
}

camera_delta phys_to_camera(phys_delta in) {
	//apply transformation matrix to get scaled_delta:
	//                  (ne)
	// (x) = ( 1  1  0) (se)
	// (y) = (-1  1 -1) (up)
	camera_delta scaled;
	scaled.x = +1 * in.ne +1 * in.se +0 * in.up;
	scaled.y = -1 * in.ne +1 * in.se -1 * in.up;

	//remove scaling factor from scaled, to get result
	//scaling factor: w/2 for x, h/2 for y
	//and the (1 << 16) fixed-point scaling factor for both.
	camera_delta result;
	result.x = scaled.x * tile_halfsize.x / ((float) (1 << 16));
	result.y = scaled.y * tile_halfsize.y / ((float) (1 << 16));

	return result;
}

camera_delta hud_to_camera(hud_delta in) {
	return camera_delta {(camera_t) in.x, (camera_t) in.y};
}

//helper for the method below
//this method works for positive d, and all signs of n
template <typename T>
int divide_round_to_closest_int(T n, T d)
{
	if (n < 0) {
		return (n - d/2)/d;
	} else {
		return (n + d/2)/d;
	}
}

tileno phys_to_tileno(phys in) {
	tileno result;
	result.ne = divide_round_to_closest_int<phys_t>(in.ne, 1 << 16);
	result.se = divide_round_to_closest_int<phys_t>(in.se, 1 << 16);
	result.up = divide_round_to_closest_int<phys_t>(in.up, 1 << 16);
	return result;
}

phys tileno_to_phys(tileno in) {
	return phys {in.ne, in.se, in.up} * (phys_t) (1 << 16);
}

} //namespace coord
} //namespace engine
} //namespace openage
