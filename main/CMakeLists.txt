set(scenario1 "scenario_1")

add_executable(${projectName} main.cpp)
target_include_directories(${projectName} PRIVATE "." "include" $ENV{CADMIUM})
target_compile_options(${projectName} PUBLIC -std=gnu++2b)

add_executable(${scenario1} ${scenario1}.cpp)
target_include_directories(${scenario1} PRIVATE "." "include" $ENV{CADMIUM})

