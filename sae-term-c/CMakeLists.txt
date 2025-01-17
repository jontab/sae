cmake_minimum_required(VERSION 3.5)
project(sae)

set(LIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/args.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/logging.c)

set(LIB_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/args.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/check.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/logging.h)

################################################################################
# Sae                                                                          #
################################################################################

set(SAE_SOURCES
    ${LIB_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c)

set(SAE_HEADERS
    ${LIB_HEADERS})

add_executable(sae
    ${SAE_SOURCES}
    ${SAE_HEADERS})

target_include_directories(sae PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/)

################################################################################
# Tests                                                                        #
################################################################################

set(TESTS_SOURCES
    ${LIB_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../deps/munit/munit.c)

set(TESTS_HEADERS
    ${LIB_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../deps/munit/munit.h)

add_executable(sae-tests
    ${TESTS_SOURCES}
    ${TESTS_HEADERS})

target_include_directories(sae-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/../deps/munit/)
