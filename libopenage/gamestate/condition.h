#ifndef CONDITION_H
#define CONDITION_H

#include <cstdint>

namespace openage {

	class Condition {
		public:
			Condition();
			~Condition();
			bool check(uint32_t gametime,uint32_t update);

	};

}

#endif // CONDITION_H
