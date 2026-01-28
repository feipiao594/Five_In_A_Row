cmake_minimum_required(VERSION 3.14)

project(FIVE_IN_A_ROW LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/debug)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/release)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
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

if(WIN32)
    add_executable(FIVE_IN_A_ROW
        WIN32
        ${SOURCE_FILES}
        resource.qrc
    )
else()
    add_executable(FIVE_IN_A_ROW
        ${SOURCE_FILES}
        resource.qrc
    )
endif()

if(APPLE)
    set_target_properties(FIVE_IN_A_ROW PROPERTIES MACOSX_BUNDLE TRUE)
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${IDENTIFIER})
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_COPYRIGHT ${COPYRIGHT})

    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${spectral_VERSION})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${spectral_VERSION})

    set(ICON_NAME "app.icns")
    set(${PROJECT_NAME}_MAC_ICON "${CMAKE_SOURCE_DIR}/${ICON_NAME}")
    set(MACOSX_BUNDLE_ICON_FILE ${ICON_NAME})
    set_property(SOURCE "${${PROJECT_NAME}_MAC_ICON}" PROPERTY
        MACOSX_PACKAGE_LOCATION Resources)
endif()

target_link_libraries(FIVE_IN_A_ROW
    Qt6::Core
    Qt6::Widgets
)
