cmake_minimum_required(VERSION 3.13)

execute_process(COMMAND tput cols
  RESULT_VARIABLE
    TPUT_RESULT
  OUTPUT_VARIABLE
    CONSOLE_WIDTH
  OUTPUT_STRIP_TRAILING_WHITESPACE)
if (TPUT_RESULT EQUAL 0)
  add_compile_definitions(CATCH_CONFIG_CONSOLE_WIDTH=${CONSOLE_WIDTH})
endif ()

add_subdirectory(formulae1)
