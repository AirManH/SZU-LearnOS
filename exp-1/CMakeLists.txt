add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/read_write_manager)

add_executable(reader_writer)

set_property(TARGET reader_writer PROPERTY AUTOMOC ON)
set_property(TARGET reader_writer PROPERTY AUTOUIC ON)

find_package(
        Qt5
        COMPONENTS Core Widgets
        REQUIRED)

target_link_libraries(reader_writer
        PRIVATE
            read_write_manager
            szu-learnos-utils
            Qt5::Core
            Qt5::Widgets)

target_sources(reader_writer
    PRIVATE
        app/main.cpp
        app/setting_dialog.ui
        app/setting_dialog.cpp
        app/setting_dialog.hpp)
