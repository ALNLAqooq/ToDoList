cmake_minimum_required(VERSION 3.16)
project(ToDoList VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Sql)

file(GLOB_RECURSE PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

file(GLOB PROJECT_RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/*.qrc
)

add_executable(ToDoList
    ${PROJECT_SOURCES}
    ${PROJECT_RESOURCES}
)

target_link_libraries(ToDoList PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Sql
)

if(MSVC)
    target_compile_options(ToDoList PRIVATE
        /utf-8
        /permissive-
    )
endif()

set_target_properties(ToDoList PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

if(WIN32)
    set_target_properties(ToDoList PROPERTIES
        OUTPUT_NAME "ToDoList"
    )
endif()

install(TARGETS ToDoList
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
