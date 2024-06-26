project(eas)

set(CMAKE_CXX_STANDARD 14)

add_executable(eas
    main.cpp
    cpu.cpp
    sched.cpp
    simulator.cpp
    task.cpp
    sched-eas.cpp
    sched-opt.cpp
)