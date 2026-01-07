set(scenario01 "scenario_01")

#add_executable(${projectName} main.cpp)
#target_include_directories(${projectName} PRIVATE "." "include" $ENV{CADMIUM})
#target_compile_options(${projectName} PUBLIC -std=gnu++2b)

add_executable(${scenario01} ${scenario01}.cpp)
target_include_directories(${scenario01} PRIVATE "." "include" $ENV{CADMIUM})

