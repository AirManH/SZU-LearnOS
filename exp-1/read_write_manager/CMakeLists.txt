add_library(read_write_manager)

find_package(
        Qt5
        COMPONENTS Core
        REQUIRED)

target_link_libraries(read_write_manager
        PRIVATE
            Qt5::Core
            szu-learnos-utils)

target_include_directories(read_write_manager
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)

target_sources(read_write_manager
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/read_write_manager.cpp)
