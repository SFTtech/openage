#ifndef OPENAGE_UTIL_FPS_H_
#define OPENAGE_UTIL_FPS_H_

namespace openage {
namespace util {

class FrameCounter {
public:
	FrameCounter();
	~FrameCounter();

	/** to be called each time a frame has been completed */
	void frame();

	/** auto-updated to always contain the current FPS value */
	float fps;

	/** contains the number of completed frames */
	unsigned count;

	/** milliseconds used for the last frame */
	unsigned int msec_lastframe;

private:
	float frame_count_weighted;
	float frame_length_sum_weighted;
	int lastframe_timestamp;
};

} //namespace util
} //namespace openage

#endif
