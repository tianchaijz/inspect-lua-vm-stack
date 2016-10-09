cmake_minimum_required(VERSION 3.0)
project(inspect-lua-vm-stack C)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")

set(_POSSIBLE_LUA_INCLUDE lua51 lua5.1 lua-5.1 luajit-2.0 luajit-2.1)
find_path(LUA_INCLUDE_DIR
    NAMES lua.h
    HINTS $ENV{LUA_DIR}
    PATHS ${CMAKE_CURRENT_SOURCE_DIR} /usr/include /usr/local/include
    PATH_SUFFIXES ${_POSSIBLE_LUA_INCLUDE}
    NO_DEFAULT_PATH
    )
if(NOT LUA_INCLUDE_DIR)
    message(FATAL_ERROR "Lua5.1 include directory not found")
else()
    message(STATUS "LUA_INCLUDE_DIR: ${LUA_INCLUDE_DIR}")
endif()

find_library(LUA
    NAMES lua5.1 lua51
    PATHS /lib /usr/lib /usr/local/lib
    )
if(NOT LUA)
    message(FATAL_ERROR "Lua5.1 library not found")
else()
    message(STATUS "LUA: ${LUA}")
endif()


if(UNIX)
    set(EXTRA_C_FLAGS "-g -O2 -fomit-frame-pointer -Wall")
    if(APPLE)
        set(LINK_FLAGS "-dynamiclib -single_module -undefined dynamic_lookup")
    else(APPLE)
        set(LINK_FLAGS "-shared")
    endif(APPLE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC ${EXTRA_C_FLAGS}")
else(UNIX)
    if(WIN32)
        add_definitions(-DLUA_BUILD_AS_DLL -DLUA_LIB)
    else(WIN32)
        message(FATAL_ERROR "unsupported platform")
    endif(WIN32)
endif(UNIX)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LUA_INCLUDE_DIR}
    )


# lua linspect module
set(LINSPECT_SRCS
    linspect.c
    )

add_library(linspect SHARED ${LINSPECT_SRCS})
set_target_properties(linspect PROPERTIES PREFIX "")
if(LINK_FLAGS)
    set_target_properties(linspect PROPERTIES
        LINK_FLAGS ${LINK_FLAGS}
        )
endif()


# lua ltrim module
set(LINSPECT_SRCS
    ltrim.c
    )

add_library(ltrim SHARED ${LINSPECT_SRCS})
set_target_properties(ltrim PROPERTIES PREFIX "")
if(LINK_FLAGS)
    set_target_properties(ltrim PROPERTIES
        LINK_FLAGS ${LINK_FLAGS}
        )
endif()


add_executable(inspect_demo inspect_demo.c)
target_link_libraries(inspect_demo ${LUA})


add_executable(bin2c util/bin2c.c)
