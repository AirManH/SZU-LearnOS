add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/read_write_manager)

add_executable(reader_writer)

find_package(
        Qt5
        COMPONENTS Core
        REQUIRED)

target_link_libraries(reader_writer
        PRIVATE
            read_write_manager
            Qt5::Core)

target_sources(reader_writer
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/app/main.cpp)
