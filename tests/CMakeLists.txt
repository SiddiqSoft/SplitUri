cmake_minimum_required(VERSION 3.21)

# install gtest
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

file(GLOB testSRC
    "*.cpp"
    )

add_executable(tests ${testSRC})

target_link_libraries(tests 
    GTest::GTest
    project
    )

include(GTest)
