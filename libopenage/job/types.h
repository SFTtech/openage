// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>

namespace openage {
namespace job {

/**
 * Type of a function that is executed by a job and returns a result.
 *
 * @param T the job's result type
 */
template<typename T>
using job_function_t = std::function<T()>;

/**
 * Type of a function that is executed by a job and returns a result. It can be
 * aborted using the two passed function objects.
 *
 * @param T the job's result type
 */
template<typename T>
using abortable_function_t = std::function<T(std::function<bool()>,std::function<void()>)>;

/**
 * Type of a function to retrieve the result of a job. If the job threw an
 * exception, it is rethrown by this function.
 *
 * @param T the job's result type
 */
template<typename T>
using result_function_t = std::function<T()>;

/**
 * Type of a job's callback function, which is called, when a job has finished.
 * The passed result function object is used to retrieve the job's result.
 *
 * @param T the job's result type
 */
template<typename T>
using callback_function_t = std::function<void(result_function_t<T>)>;

/** Type of a function that returns whether a job should be aborted. */
using should_abort_t = std::function<bool()>;

/** Type of a function that aborts a job. */
using abort_t = std::function<void()>;

}
}
