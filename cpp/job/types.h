// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_JOB_TYPES_H_
#define OPENAGE_JOB_TYPES_H_

#include <functional>

namespace openage {
namespace job {

template<typename T>
using job_function_t = std::function<T()>;

using should_abort_t = std::function<bool()>;
using abort_t = std::function<void()>;

template<typename T>
using abortable_function_t = std::function<T(std::function<bool()>,std::function<void()>)>;

template<typename T>
using result_function_t = std::function<T()>;

template<typename T>
using callback_function_t = std::function<void(result_function_t<T>)>;

}
}

#endif
