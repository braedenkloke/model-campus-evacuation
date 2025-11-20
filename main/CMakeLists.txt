add_executable(${projectName} main.cpp)

target_include_directories(${projectName} PRIVATE "." "include" $ENV{CADMIUM})

target_compile_options(${projectName} PUBLIC -std=gnu++2b)
