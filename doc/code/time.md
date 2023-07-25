# Simulation Time

- basis for all game simulation and rendering routines
  - scheduling and execution of events (event loop)
  - data management over time (curves)
  - timing animations (renderer)

![time loop UML]()

- time type
  - curve::time_t
  - fixed point value
  - time unit: seconds

- controlled by time loop
  - advances time with clock
  - runs in its own thread
  - accessed by simulation and renderer threads

- clock manages time
  - calculates time using system clock
  - can be stopped and resumed
  - tracks both real time and simulation time
    - real time = cumulative run time of clock
    - speed can be adjusted for simulation time
    - simulationm time = cumulative run time of clock adjusted by speed
