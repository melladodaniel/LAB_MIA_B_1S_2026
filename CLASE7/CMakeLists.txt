cmake_minimum_required(VERSION 3.10)
project(Clase7)

set(CMAKE_CXX_STANDARD 17)

add_executable(Clase7
    main.cpp
    Analyzer/Analyzer.cpp
    DiskManagement/DiskManagement.cpp
    Utilities/Utilities.cpp
)
