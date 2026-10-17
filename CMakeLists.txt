cmake_minimum_required(VERSION 3.20)
project(MarkdownToHTML)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(converter)

add_executable(MarkdownToHTML main.cpp)

target_link_libraries(
        MarkdownToHTML
        PRIVATE
        MarkdownConverter
)

include(CTest)
if(BUILD_TESTING)
    add_executable(MarkdownConverterTests tests.cpp)
    target_link_libraries(MarkdownConverterTests PRIVATE MarkdownConverter)
    add_test(NAME MarkdownConverterTests COMMAND MarkdownConverterTests)
    add_test(NAME MarkdownToHTMLFixtures COMMAND ${CMAKE_COMMAND}
            -DPROGRAM=$<TARGET_FILE:MarkdownToHTML>
            -DTEST_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests
            -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/verify.cmake)
endif()
