// Copyright 2015-2025 the openage authors. See copying.md for legal info.

#include "tests.h"

#include "log/log.h"
#include "log/message.h"

#include "renderer/demo/demo_0.h"
#include "renderer/demo/demo_1.h"
#include "renderer/demo/demo_2.h"
#include "renderer/demo/demo_3.h"
#include "renderer/demo/demo_4.h"
#include "renderer/demo/demo_5.h"
#include "renderer/demo/demo_6.h"
#include "renderer/demo/demo_7.h"
#include "renderer/demo/stresstest_0.h"
#include "renderer/demo/stresstest_1.h"

namespace openage::renderer::tests {

void renderer_demo(int demo_id, const util::Path &path) {
	switch (demo_id) {
	case 0:
		renderer_demo_0(path);
		break;

	case 1:
		renderer_demo_1(path);
		break;

	case 2:
		renderer_demo_2(path);
		break;

	case 3:
		renderer_demo_3(path);
		break;

	case 4:
		renderer_demo_4(path);
		break;

	case 5:
		renderer_demo_5(path);
		break;

	case 6:
		renderer_demo_6(path);
		break;

	case 7:
		renderer_demo_7(path);
		break;

	default:
		log::log(MSG(err) << "Unknown renderer demo requested: " << demo_id << ".");
		break;
	}
}

OAAPI void renderer_stresstest(int demo_id, const util::Path &path) {
	switch (demo_id) {
	case 0:
		renderer_stresstest_0(path);
		break;
	case 1:
		renderer_stresstest_1(path);
		break;
	default:
		log::log(MSG(err) << "Unknown renderer stresstest requested: " << demo_id << ".");
		break;
	}
}

} // namespace openage::renderer::tests
