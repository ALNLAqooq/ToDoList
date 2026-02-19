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
    src/controllers/backupmanager.cpp
    src/controllers/notificationmanager.cpp
    src/utils/logger.cpp
    src/utils/date_utils.cpp
    src/utils/file_utils.cpp
    src/utils/theme_utils.cpp
    src/utils/icon_utils.cpp
    src/utils/theme_manager.cpp
    src/controllers/task_controller.cpp
    src/models/task.cpp
    src/models/task_step.cpp
    src/models/tag.cpp
    src/models/taskmodel.cpp
    src/models/notification.cpp
    src/models/folder.cpp
)

set(HEADERS
    src/app.h
    src/controllers/database.h
    src/controllers/backupmanager.h
    src/controllers/notificationmanager.h
    src/utils/logger.h
    src/utils/date_utils.h
    src/utils/file_utils.h
    src/utils/theme_utils.h
    src/utils/icon_utils.h
    src/utils/theme_manager.h
    src/controllers/task_controller.h
    src/models/task.h
    src/models/task_step.h
    src/models/tag.h
    src/models/taskmodel.h
    src/models/notification.h
    src/models/folder.h
    src/models/task_search_filters.h
)

set(UI_SOURCES
    src/views/mainwindow.cpp
    src/views/sidebar.cpp
    src/views/content_area.cpp
    src/views/empty_state_widget.cpp
    src/views/search_widget.cpp
    src/views/settingsdialog.cpp
    src/views/task_list_widget.cpp
    src/views/task_card_widget.cpp
    src/views/task_dialog.cpp
    src/views/task_tree.cpp
    src/views/task_detail_widget.cpp
    src/views/notificationpanel.cpp
)

set(UI_HEADERS
    src/views/mainwindow.h
    src/views/sidebar.h
    src/views/content_area.h
    src/views/empty_state_widget.h
    src/views/search_widget.h
    src/views/settingsdialog.h
    src/views/task_list_widget.h
    src/views/task_card_widget.h
    src/views/task_dialog.h
    src/views/task_tree.h
    src/views/task_detail_widget.h
    src/views/notificationpanel.h
)

set(RESOURCES
    resources/icons.qrc
    resources/styles.qrc
)

add_executable(ToDoList WIN32 ${SOURCES} ${UI_SOURCES} ${RESOURCES})

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

# CMAKE_AUTORCC is ON, so including .qrc files in the target is enough.

if(WIN32)
    set_target_properties(ToDoList PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

install(TARGETS ToDoList
    RUNTIME DESTINATION .
)
