# Code Optimization

openage should be efficient in its resource usage and offer high performance.
As we are building a game engine, it is kind of expected that everything runs
smoothly out-of-the-box.

1. [Basic Rules](#basic-rules)
2. [Checking Performance](#checking-performance)


## Basic Rules

Before anyone dives into the codebase and starts changing things around for better performance,
they should consider these simple rules:

- **readability comes first**: openage depends on outsiders being able to understand the engine code. If the code is super fast but totally incomprehensible, then nobody is going to maintain it.
- **avoid premature optimization**: optimization without reason wastes time, reduces readability, and can sometimes lead to even worse performance. It's often better to implement a workable solution first and then use profiling to identify bottlenecks. Modern compilers are often smart enough to find small improvements during compilation anyway.
- **prefer architecture improvements over speeding up specific use cases**: openage is a game *engine*, so we don't exactly know what people will use it for. We want the general case to be fast, not the edge cases.


## Checking Performance

Performance bottlenecks are best identified with a *profiler* of your choice.

For **Python**, the built-in modules [cProfile](https://docs.python.org/3/library/profile.html)
(speed) and [tracemalloc](https://docs.python.org/3/library/tracemalloc.html) (memory) are recommended.
openage also provides convenience functions for them in its `openage.util.profiler` module.

For **C++**, [valgrind](https://valgrind.org/info/tools.html) with `callgrind` (speed) and
`memcheck` (memory) can be used. If you use valgrind, you should supply it the suppression
file in our repo: [`etc/valgrind-python.supp`](/etc/valgrind-python.supp). This lets
valgrind handle the Python parts of the engine a bit better.

Any other profiler will also do the trick as long as you know how to use it (e.g. `perf`,
`VTune`, `uProf`, etc.).

Profiling a full engine run can be very resource intensive to profile. Therefore, it
can be beneficial to profile engine [demos](/doc/code/testing.md#demos) for a specific
subsystem, e.g. the renderer or the event system. This is also useful to
check the performance of subsystems in isolation. However, consider that a full engine
run may have different bottlenecks that are not obvious in the demos.

Stresstests are demos that are specifically designed to test performance, particularly
in the renderer subsystem. They can also include benchmarks with additional
information, e.g. an FPS counter. When profiling performance critical code, implementing
a stresstest should be considered to complement profiling results.
