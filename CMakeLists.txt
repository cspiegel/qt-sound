cmake_minimum_required(VERSION 3.16)

project(qt-sound LANGUAGES CXX)

option(WITH_QT6 "Use Qt6" OFF)
if(WITH_QT6)
    set(QT_VERSION "6")
else()
    set(QT_VERSION "5")
endif()

set(CMAKE_AUTOMOC ON)

find_package(Qt${QT_VERSION} REQUIRED COMPONENTS Widgets Multimedia)

add_executable(qt-sound qt-sound.cpp)

target_link_libraries(qt-sound PRIVATE Qt${QT_VERSION}::Widgets Qt${QT_VERSION}::Multimedia)
