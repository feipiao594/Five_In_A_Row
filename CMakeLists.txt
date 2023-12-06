cmake_minimum_required(VERSION 3.14)

project(WUZIQI LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/debug)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/release)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.h
    ${CMAKE_SOURCE_DIR}/src/*.hpp
    ${CMAKE_SOURCE_DIR}/src/controller/*.cpp
    ${CMAKE_SOURCE_DIR}/src/controller/*.h
    ${CMAKE_SOURCE_DIR}/src/controller/*.hpp
    ${CMAKE_SOURCE_DIR}/src/ui/*.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/*.h
    ${CMAKE_SOURCE_DIR}/src/ui/*.hpp
)


add_executable(WUZIQI
    ${SOURCE_FILES}
    resource.qrc
)
target_link_libraries(WUZIQI
    Qt6::Core
    Qt6::Widgets
)
