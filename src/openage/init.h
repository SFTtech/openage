#ifndef _OPENAGE_INIT_H_
#define _OPENAGE_INIT_H_


#include "../engine/util/dir.h"

namespace openage {

void init(engine::util::Dir &data_dir);
void destroy();

} //namespace openage

#endif //_OPENAGE_INIT_H_
