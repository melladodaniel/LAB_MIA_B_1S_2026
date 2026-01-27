cmake_minimum_required(VERSION 3.10)
project(clase1)

set(CMAKE_CXX_STANDARD 17)

add_executable(clase1
    main.cpp
)

target_include_directories(clase1 PRIVATE
    ${PROJECT_SOURCE_DIR}/external

target_link_libraries(clase1 pthread)
)