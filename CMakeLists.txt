cmake_minimum_required(VERSION 3.3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(lwp)

set(SOURCE_FILES
	src/main.c
	src/wlp.c
	src/wlp_layers.c
	src/wlpapi.c
	src/log.c
	src/dict.c
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

#lua dependency
find_package(Lua51 REQUIRED)
list(APPEND _INCLUDE_DIRS ${LUA_INCLUDE_DIRS})
list(APPEND _LIBS ${LUA_LIBRARIES})

# SDL3 dependency
find_package(SDL3 REQUIRED CONFIG)
list(APPEND _INCLUDE_DIRS ${SDL3_INCLUDE_DIRS})
list(APPEND _LIBS ${SDL3_LIBRARIES})

if (_UNAME STREQUAL "LINUX")
  # X11 and Xrandr dependency
  include(FindX11)

  list(APPEND _INCLUDE_DIRS ${X11_INCLUDE_DIR})
  list(APPEND _LIBS ${X11_LIBRARIES})

  list(APPEND _INCLUDE_DIRS ${X11_Xrandr_INCLUDE_PATH})
  list(APPEND _LIBS ${X11_Xrandr_LIB})
endif()

add_executable(lwp ${SOURCE_FILES})

target_compile_definitions(lwp PUBLIC __${_UNAME})
target_include_directories(lwp PUBLIC ${_INCLUDE_DIRS})
target_link_libraries(lwp PRIVATE ${_LIBS})
