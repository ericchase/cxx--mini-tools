cmake_minimum_required(VERSION 3.29.3)
project("mkf" VERSION 0.1.0 LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 26)

include(CTest)
enable_testing()

add_executable(${PROJECT_NAME} main.cxx)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E $<IF:$<CONFIG:Release>,copy,true> $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_SOURCE_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
