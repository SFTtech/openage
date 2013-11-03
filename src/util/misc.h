#ifndef _UTIL_MISC_H_
#define _UTIL_MISC_H_
namespace openage {
namespace util {

/**
returns a random number in the given range
@param lower the (included) lower limit of the generated number
@param upper the (also included) upper limit of the generated number
*/
int random_range(int lower, int upper);

} //namespace util
} //namespace openage

#endif //_UTIL_MISC_H_
