cmake_minimum_required(VERSION 3.12)

project(lwp
    LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

find_package(sdl2 REQUIRED CONFIG)
set(_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})
set(_LIBS ${SDL2_LIBRARIES})

if (_UNAME STREQUAL "LINUX")
  find_package(X11 REQUIRED)
  list(APPEND _INCLUDE_DIRS ${X11_INCLUDE_DIR})
  list(APPEND _LIBS ${X11_LIBRARIES})
endif()

add_executable(lwp
    ${_SOURCE_FILES}
    )
target_compile_definitions(lwp PUBLIC __${_UNAME})
target_include_directories(lwp PUBLIC ${_INCLUDE_DIRS})
target_link_libraries(lwp PRIVATE ${_LIBS})
