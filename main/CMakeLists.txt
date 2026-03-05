add_executable(${projectName} main.cpp)
target_include_directories(${projectName} PRIVATE "." "include" $ENV{CADMIUM})
target_compile_options(${projectName} PUBLIC -std=gnu++2b)

add_executable(intersection_test intersection_test.cpp)
target_include_directories(intersection_test PRIVATE "." "include" $ENV{CADMIUM})
target_compile_options(intersection_test PUBLIC -std=gnu++2b)


