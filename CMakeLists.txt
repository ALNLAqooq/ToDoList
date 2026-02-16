cmake_minimum_required(VERSION 3.16)
project(ToDoList VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_compile_options(/utf-8)

find_package(Qt5 COMPONENTS Core Gui Widgets Sql Svg REQUIRED)

set(SOURCES
    src/main.cpp
    src/app.cpp
    src/controllers/database.cpp
    src/utils/logger.cpp
    src/utils/date_utils.cpp
    src/utils/file_utils.cpp
    src/utils/theme_utils.cpp
    src/utils/icon_utils.cpp
    src/utils/theme_manager.cpp
    src/controllers/task_controller.cpp
    src/models/task.cpp
    src/models/tag.cpp
)

set(HEADERS
    src/app.h
    src/controllers/database.h
    src/utils/logger.h
    src/utils/date_utils.h
    src/utils/file_utils.h
    src/utils/theme_utils.h
    src/utils/icon_utils.h
    src/utils/theme_manager.h
    src/controllers/task_controller.h
    src/models/task.h
    src/models/tag.h
)

set(UI_SOURCES
    src/views/mainwindow.cpp
    src/views/task_list_widget.cpp
    src/views/task_card_widget.cpp
)

set(UI_HEADERS
    src/views/mainwindow.h
    src/views/task_list_widget.h
    src/views/task_card_widget.h
)

add_executable(ToDoList WIN32 ${SOURCES} ${HEADERS} ${UI_SOURCES} ${UI_HEADERS})

target_link_libraries(ToDoList PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Sql Qt5::Svg)

set_target_properties(ToDoList PROPERTIES
    WIN32_EXECUTABLE TRUE
    MFC_RUNTIME_LIBRARY FALSE
)

if(MSVC)
    set_target_properties(ToDoList PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data"
        OUTPUT_NAME "ToDoList"
    )
endif()

set(RESOURCES
    resources/icons.qrc
    resources/styles.qrc
)

qt5_add_resources(ToDoList "${RESOURCES}")

if(WIN32)
    set_target_properties(ToDoList PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

install(TARGETS ToDoList
    RUNTIME DESTINATION .
)
