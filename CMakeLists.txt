# CMakeList.txt: CityJSON 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project ("CityJSON")

# set(CMAKE_CXX_COMPILER g++)
# set(CMAKE_CXX_FLAGS "-Wall -std=c++11")

# set(${PROJECT_NAME}_VERSION_MAJOR 1)
# set(${PROJECT_NAME}_VERSION_MINOR 0)

include_directories( ${CMAKE_SOURCE_DIR}/include/ ) # include json.hpp

add_definitions(
  -DDATA_PATH=\"${PROJECT_SOURCE_DIR}/data\"
)

add_executable (CityJSON "src/main.cpp"  "src/calculation.hpp" "src/memo.h")

target_link_libraries(${PROJECT_NAME})
# TODO: 如有需要，请添加测试并安装目标。
