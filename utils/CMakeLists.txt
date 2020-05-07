set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_library(szu-learnos-utils)

find_package(
        Qt5
        COMPONENTS Core
        REQUIRED)

set_property(TARGET szu-learnos-utils PROPERTY AUTOMOC ON)

target_link_libraries(szu-learnos-utils PRIVATE Qt5::Core)

target_include_directories(szu-learnos-utils PUBLIC include/)

target_sources(szu-learnos-utils
    PRIVATE
        src/szu-learnos-utils.cpp
        include/szu-learnos-utils.hpp)

