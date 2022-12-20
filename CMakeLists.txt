cmake_minimum_required(VERSION 3.0.0)
project(SplitUri VERSION 0.1.0)

include(GTest)
enable_testing()

add_library(SplitUri 
    "src/*.hpp"
    )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
