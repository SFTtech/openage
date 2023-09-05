# Simulation Time

*Simulation time* is the driving force behind pretty much all internal routines of the engine.
Its used to schedule and execute events in the [event system](event_system.md),
managing data of game entities at runtime via [curves](curves.md), and time
animations in the [renderer](/doc/code/renderer/level2.md).

To keep track of and synchronize time between different engine components, openage implements
a low-level time control interface in the `openage::time` namespace. This system is very simple:
Basically, it consists of a loop in a dedicated thread that constantly updates an internal clock.

![time loop UML](images/time_classes_uml.svg)

`TimeLoop` provides a basic interface to create an internal clock and run the update loop. Once
initialized, the `Clock` instance can be accessed with the `get_clock()` method. For most calculations,
the clock object is sufficient. Access to the time loop is only necessary if you want to
pause/stop the entire simulation.

openage represents time with the internal type `time::time_t` (which must not be confused with the C standard
type `time_t`). `time::time_t` is a 64 bit signed fixed-point value (scaling factor 16)
which represents the time passed in **seconds**, i.e. a value of `1.25` denotes 1.25 seconds or
1250 milliseconds, respectively.

`Clock` manages the current simulation time using the system clock. It also allows the configuration
of a simulation speed (which can be negative). Time is advanced in each
loop iteration of `TimeLoop` by calling the clock object's `update_time()` method. This method
first calculates the real time difference between the previous and current update call. The result
is then multiplied by the simulation speed and added to the total simulation time. Additionally, `Clock`
keeps track of how much real time has passed, regardless of simulation speed. Real time is used
in the renderer to time animation frames which should not affected by simulation speed.

The time resolution used for the system clock is **milliseconds**. When requesting the current time
from a clock via `get_time()` or `get_real_time()`, this gets converted into the openage `time::time_t`
format.

To prevent the clock updates from advancing too far if the program is stopped by the operating system
or a debugger, `update_time()` sets a maximum limit on how much real time can pass between two update
calls. This limit is currently hardcoded to 50ms. Similarly, there is a minimum limit of 1ms to
prevent the time loop thread from stalling.
