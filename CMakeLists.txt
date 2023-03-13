cmake_minimum_required(VERSION 3.13)

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

if (_UNAME STREQUAL "DARWIN")
  # MacOSX framework dependencies
  list(APPEND _LIBS "-framework CoreGraphics" "-framework Foundation")
endif()

if (_UNAME STREQUAL "LINUX")
  # X11 dependency
  find_package(X11 REQUIRED)
  list(APPEND _INCLUDE_DIRS ${X11_INCLUDE_DIR})
  list(APPEND _LIBS ${X11_LIBRARIES})
endif()

option(LWP_INSTALL_LAUNCHD "Launch lwp on login (MacOSX only)" OFF)

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

if (_UNAME STREQUAL "DARWIN")
  # MacOSX specific installation rules
  execute_process(COMMAND whoami
      OUTPUT_VARIABLE _USERNAME)

  # The plist file needs absolute path to the binary. In order to
  # do so we use `configure_file` to replace place holders in the
  # template with the needed variable. However, users might change
  # the install prefix during the installation time, and to account
  # for that we provide a cmake code snippet instead of populating
  # the install prefix prematurely during configuration time.
  install(CODE "
    set(user ${_USERNAME})
    set(binpath \${CMAKE_INSTALL_PREFIX}/bin/lwp)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lwp.template.plist
        \${CMAKE_INSTALL_PREFIX}/lwp.plist
        @ONLY)

    # Install to launchd in order to run lwp on login
    if (LWP_INSTALL_LAUNCHD)
      message(STATUS \"Installing to launchd...\")
      execute_process(
        COMMAND mkdir -p ~/Library/LaunchAgents
        )
      execute_process(
        COMMAND cp \${CMAKE_INSTALL_PREFIX}/lwp.plist ~/Library/LaunchAgents/
        )
      execute_process(
        COMMAND launchctl load ~/Library/LaunchAgents/lwp.plist
        )
    endif()
    ")
endif()
