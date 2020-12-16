cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

project(touchviz)


# requirements
find_package(glfw3 3.3 REQUIRED)
find_package(Libevdev REQUIRED)

# set include paths
include_directories(${LIBEVDEV_INCLUDE_DIRS})
include_directories(external/glad/include)
include_directories(external/spdlog/include)
include_directories(external/argh)
include_directories(external/glm)

# include the source folder
include_directories(src)

add_executable(touchviz src/main.cpp)
target_link_libraries(touchviz glfw ${LIBEVDEV_LIBRARY})