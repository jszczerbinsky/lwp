cmake_minimum_required(VERSION 3.12)

project(lwp
    LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Detect the platform
if (APPLE)
  set(_UNAME "DARWIN")
elseif (WIN32)
  set(_UNAME "WIN32")
else()
  set(_UNAME "LINUX")
endif()

set(_SOURCE_FILES
    debug.c
    main.c
    parser.c
    wallpaper.c
    window.c
    )

# SDL2 dependency
find_package(sdl2 REQUIRED CONFIG)
set(_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})
set(_LIBS ${SDL2_LIBRARIES})

if (_UNAME STREQUAL "LINUX")
  # X11 dependency
  find_package(X11 REQUIRED)
  list(APPEND _INCLUDE_DIRS ${X11_INCLUDE_DIR})
  list(APPEND _LIBS ${X11_LIBRARIES})
endif()

# Config file
set(_DEFAULT_CONFIG_FILE default.cfg)
if (_UNAME STREQUAL "DARWIN")
  set(_DEFAULT_CONFIG_FILE defaultMac.cfg)
endif()

# Main executable
add_executable(lwp
    ${_SOURCE_FILES}
    )
target_compile_definitions(lwp PUBLIC __${_UNAME})
target_include_directories(lwp PUBLIC ${_INCLUDE_DIRS})
target_link_libraries(lwp PRIVATE ${_LIBS})

# Installation rules
install(TARGETS lwp)
install(DIRECTORY wallpapers
    DESTINATION share/lwp)
install(FILES LICENSE
    DESTINATION share/lwp)
install(FILES ${_DEFAULT_CONFIG_FILE}
    TYPE SYSCONF
    RENAME lwp.cfg)
