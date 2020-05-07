add_library(read_write_manager)

set_property(TARGET read_write_manager PROPERTY AUTOMOC ON)

find_package(
        Qt5
        COMPONENTS Core
        REQUIRED)

target_link_libraries(read_write_manager
        PRIVATE
            Qt5::Core
            szu-learnos-utils)

target_include_directories(read_write_manager
        PUBLIC include/)

target_sources(read_write_manager
    PRIVATE
        src/read_write_manager.cpp
        include/read_write_manager.hpp)
