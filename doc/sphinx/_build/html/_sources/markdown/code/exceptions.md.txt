The openage base exception type is `error::Error`.

 - The first argument is a `message` object; the usage is identical to `log::log()` (i.e. preferably via the `MSG` macro).
 - If `true` is given as the second argument (default: `false`), a stack trace is collected, using gcc 4.9's `libbacktrace.a`.
The collection itself is quite fast and has a low performance impact, as no symbol look-ups/string translations happen at that time.
 - If `true` is given as the third argument (default: `true`), and currently inside a `catch` block, the current exception is stored as a cause, and can later be re-thrown via `rethrow_cause()`.

Exceptions should always be caught by reference, or inheritance won't work.

Example usage:

    #include "error/error.h"

    try {
        int i = 5;

         try {
            if (true) {
                throw Error(MSG(err) << "what an exceptional line of code!", true);
            }
            i = 6;
        } catch (...) {
            throw Error(MSG(crit).fmt("exception in the 'try' block. i=%d", i));
        }

    } catch (Error &e) {
        std::cout << e << std::endl;
    }

Also see the example demo code in `cpp/error/demo.cpp`.

Avoid exceptions that don't inherit from `Error`.
