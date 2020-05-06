add_library(szu-learnos-utils)

find_package(
        Qt5
        COMPONENTS Core
        REQUIRED)

target_link_libraries(szu-learnos-utils PRIVATE Qt5::Core)

target_include_directories(szu-learnos-utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)

target_sources(szu-learnos-utils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/szu-learnos-utils.cpp)

