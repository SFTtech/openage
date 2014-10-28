#ifndef OPENAGE_UTIL_FPS_H_B8948646A9F248F89FF02670DBE0614C
#define OPENAGE_UTIL_FPS_H_B8948646A9F248F89FF02670DBE0614C

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

#endif //_UTIL_FPS_H_
