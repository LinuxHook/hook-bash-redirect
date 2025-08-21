cmake_minimum_required(VERSION 3.16)
project(hook-bash-redirect C)

set(CMAKE_C_STANDARD 11)

option(DEBUG "Enable syslog" ON)

add_definitions(
        -D C_STRLEN
        -D C_STR_HAS_PREFIX
        -D C_STR_HAS_SUFFIX
)

if (DEBUG)
    add_definitions(-D DEBUG)
endif ()

find_package(PkgConfig)

add_library(hook-bash-redirect SHARED
        hook-bash-redirect.c
        hook-bash-redirect-helper.c

        hook-common/hook-common.h hook-common/hook-common.c
)
target_link_libraries(hook-bash-redirect PUBLIC
        -pthread)

target_include_directories(hook-bash-redirect PUBLIC
        ${CMAKE_SOURCE_DIR}/3thrd/macros
)
target_compile_options(hook-bash-redirect PUBLIC -fPIC)
set_target_properties(hook-bash-redirect PROPERTIES OUTPUT_NAME "hook-bash-redirect" PREFIX "")