cmake_minimum_required(VERSION 3.15)
project(lcs)
set (CMAKE_CXX_STANDARD 17)
add_executable(lcs 
    main.cpp 
    game.cpp 
    mojoelf.cpp 
    jni_patch.cpp
)

add_subdirectory(FalsoJNI)
target_link_libraries(lcs PRIVATE falsojni mpg123 glfw stdc++)
set_property(TARGET lcs PROPERTY ENABLE_EXPORTS ON)