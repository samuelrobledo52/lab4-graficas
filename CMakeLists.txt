cmake_minimum_required(VERSION 3.16)
project(SoftwareRenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(SDL2 REQUIRED)

add_executable(renderer
    src/main.cpp
    src/framebuffer.cpp
    src/line.cpp
    src/triangle.cpp
    src/objloader.cpp
)

target_include_directories(renderer PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(TARGET SDL2::SDL2)
    target_link_libraries(renderer PRIVATE SDL2::SDL2)
else()
    target_include_directories(renderer SYSTEM PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(renderer PRIVATE ${SDL2_LIBRARIES})
endif()

if(MSVC)
    target_compile_options(renderer PRIVATE /W4)
else()
    target_compile_options(renderer PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(renderer PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
