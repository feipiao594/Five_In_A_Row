cmake_minimum_required(VERSION 3.14)

project(WUZIQI LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.h
    ${CMAKE_SOURCE_DIR}/src/*.hpp
)


add_executable(WUZIQI
  ${SOURCE_FILES}
  model/board.cpp model/board.h model/coordinate.cpp model/coordinate.h model/manager.cpp model/manager.h model/player.cpp model/player.h
  model/computer.h model/computer.cpp
)
target_link_libraries(WUZIQI
    Qt6::Core
    Qt6::Widgets
)
