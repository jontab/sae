cmake_minimum_required(VERSION 3.5)
project(sae)

set(LIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/components/buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/components/status.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/events/action.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/events/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/text/buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/text/char.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/args.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/editor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/logging.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/view.c)

set(LIB_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/components/buffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/components/component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/components/status.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/events/action.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/events/event.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/events/queue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/objects/point.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/text/buffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/text/char.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/args.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/check.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/editor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/logging.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/view.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../deps/termbox2/termbox2.h)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/../deps/termbox2/)

target_compile_options(sae PUBLIC
    -Wall
    -Werror
    -O3)

################################################################################
# Tests                                                                        #
################################################################################

set(TESTS_SOURCES
    ${LIB_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit_char.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit_queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../deps/munit/munit.c)

set(TESTS_HEADERS
    ${LIB_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../deps/munit/munit.h)

add_executable(sae-tests
    ${TESTS_SOURCES}
    ${TESTS_HEADERS})

target_include_directories(sae-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/../deps/munit/
    ${CMAKE_CURRENT_SOURCE_DIR}/../deps/termbox2/)

target_compile_options(sae-tests PUBLIC
    -Wall
    -Werror
    -O0
    -g)
