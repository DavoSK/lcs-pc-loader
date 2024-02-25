cmake_minimum_required(VERSION 3.15)
project(lcs)
set (CMAKE_CXX_STANDARD 17)
add_executable(lcs 
    main.cpp 
    game.cpp 
    mojoelf.cpp 
    jni_patch.cpp
    falsojni_impl.cpp
)

add_subdirectory(FalsoJNI)
add_subdirectory(subhook)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
target_link_libraries(lcs PRIVATE falsojni pthread subhook mpg123 glfw stdc++)
set_property(TARGET lcs PROPERTY ENABLE_EXPORTS ON)