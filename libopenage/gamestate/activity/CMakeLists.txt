add_sources(libopenage
    activity.cpp
    end_node.cpp
    node.cpp
    start_node.cpp
    task_node.cpp
    task_system_node.cpp
    tests.cpp
    types.cpp
    xor_event_gate.cpp
    xor_gate.cpp
)

add_subdirectory("event")
add_subdirectory("condition")
