execute_process(
        COMMAND "${PROGRAM}" --in "${TEST_DIR}/tests.md"
        RESULT_VARIABLE stdout_result
        OUTPUT_FILE "${OUTPUT_DIR}/tests.actual.txt"
)
if(NOT stdout_result EQUAL 0)
    message(FATAL_ERROR "CLI stdout mode failed: ${stdout_result}")
endif()
file(READ "${OUTPUT_DIR}/tests.actual.txt" stdout_html)
file(READ "${TEST_DIR}/tests.txt" expected_stdout)
string(REPLACE "\r\n" "\n" stdout_html "${stdout_html}")
string(REPLACE "\r\n" "\n" expected_stdout "${expected_stdout}")
if(NOT stdout_html STREQUAL expected_stdout)
    message(FATAL_ERROR "CLI stdout differs from tests.txt")
endif()

set(output_file "${OUTPUT_DIR}/tests.actual.html")
execute_process(
        COMMAND "${PROGRAM}" --in "${TEST_DIR}/tests.md" --out "${output_file}"
        RESULT_VARIABLE file_result
)
if(NOT file_result EQUAL 0)
    message(FATAL_ERROR "CLI file mode failed: ${file_result}")
endif()
file(READ "${TEST_DIR}/tests.html" expected_file)
file(READ "${output_file}" actual_file)
string(REPLACE "\r\n" "\n" actual_file "${actual_file}")
string(REPLACE "\r\n" "\n" expected_file "${expected_file}")
if(NOT actual_file STREQUAL expected_file)
    message(FATAL_ERROR "CLI file output differs from tests.html")
endif()

execute_process(
        COMMAND "${PROGRAM}" --in "${TEST_DIR}/empty.md"
        RESULT_VARIABLE empty_stdout_result
        OUTPUT_FILE "${OUTPUT_DIR}/empty.actual.txt"
)
if(NOT empty_stdout_result EQUAL 0)
    message(FATAL_ERROR "CLI failed on an empty input file")
endif()
file(SIZE "${OUTPUT_DIR}/empty.actual.txt" empty_stdout_size)
if(NOT empty_stdout_size EQUAL 0)
    message(FATAL_ERROR "Empty input should produce empty stdout")
endif()
