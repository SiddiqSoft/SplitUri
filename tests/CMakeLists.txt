if(${${PROJECT_NAME}_BUILD_TESTS})
    set(TESTPROJ ${PROJECT_NAME}_tests)

    add_executable(${TESTPROJ})

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fprofile-arcs -ftest-coverage -fprofile-instr-generate -fcoverage-mapping")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -fprofile-arcs -ftest-coverage -fprofile-instr-generate -fcoverage-mapping")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fprofile-instr-generate")

    target_compile_features(${TESTPROJ} PRIVATE cxx_std_17)
    target_compile_options( ${TESTPROJ}
                            PRIVATE
                            $<$<CXX_COMPILER_ID:MSVC>:/std:c++17> )
    target_sources( ${TESTPROJ}
                    PRIVATE
                    ${PROJECT_SOURCE_DIR}/tests/examples.cpp
                    ${PROJECT_SOURCE_DIR}/tests/test_splituri_narrow.cpp
                    ${PROJECT_SOURCE_DIR}/tests/test_splituri_wide_nojson.cpp
                    ${PROJECT_SOURCE_DIR}/tests/test_splituri_wide.cpp)

    # Dependencies
    cpmaddpackage("gh:google/googletest#v1.15.2")
    target_link_libraries(${TESTPROJ} PRIVATE GTest::gtest_main)
    cpmaddpackage("gh:nlohmann/json#v3.11.3")
    target_link_libraries(${TESTPROJ} PRIVATE nlohmann_json::nlohmann_json)


    include(GoogleTest)

    gtest_discover_tests(${TESTPROJ} XML_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/tests/results")

    include(CTest)
    message(STATUS "  Finished configuring for ${PROJECT_NAME} -- ${PROJECT_NAME}_BUILD_TESTS = ${${PROJECT_NAME}_BUILD_TESTS}")
endif()
