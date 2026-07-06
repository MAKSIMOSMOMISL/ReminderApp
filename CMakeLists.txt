cmake_minimum_required(VERSION 3.16)
project(ReminderApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg)

add_executable(ReminderApp
    src/main.cpp
    src/ReminderManager.cpp
    src/ReminderManager.h
    src/ReminderWidget.cpp
    src/ReminderWidget.h
    resources.rc
)

target_link_libraries(ReminderApp Qt6::Core Qt6::Widgets Qt6::Svg)

if(WIN32)
    set_target_properties(ReminderApp PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
