#pragma once
// pxd: from libopenage.util.path cimport Path
#include "../util/path.h"


namespace openage {
namespace renderer {
namespace batch_test{
 // pxd: void batch_demo(int demo_id, Path path) except +
void batch_demo(int demo_id,util::Path path);
}}} // openage::renderer::batch_test