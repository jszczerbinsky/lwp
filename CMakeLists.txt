cmake_minimum_required(VERSION 3.13)

project(lwp
    LANGUAGES C)
		
string(REPLACE "\\" "/" sdl2_RUNTIME_DIR ${sdl2_RUNTIME_DIR})

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
		
# Windows resource file
if(_UNAME STREQUAL "WIN32")
	list(APPEND _SOURCE_FILES "resource.rc")
endif()

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
if(_UNAME STREQUAL "WIN32")
	set(_DEFAULT_CONFIG_FILE defaultWin.cfg)
endif()
if (_UNAME STREQUAL "DARWIN")
  set(_DEFAULT_CONFIG_FILE defaultMac.cfg)
endif()

# Main executable
add_executable(lwp ${_SOURCE_FILES})

# Windows specific properties for executable
if(_UNAME STREQUAL "WIN32")
	set_property(TARGET lwp PROPERTY VS_DPI_AWARE "PerMonitor")
endif()

target_compile_definitions(lwp PUBLIC __${_UNAME})
target_include_directories(lwp PUBLIC ${_INCLUDE_DIRS})
target_link_libraries(lwp PRIVATE ${_LIBS})

# Installation rules
if(_UNAME STREQUAL "WIN32")
	install(TARGETS lwp
			DESTINATION .)
	install(DIRECTORY wallpapers
			DESTINATION .)
	install(FILES LICENSE.txt
			DESTINATION .)
	install(FILES ${_DEFAULT_CONFIG_FILE}
			DESTINATION .)
	install(FILES ${sdl2_RUNTIME_DIR}/SDL2.dll
			DESTINATION .)
	install(FILES ${sdl2_RUNTIME_DIR}/README-SDL.txt
			DESTINATION .)
else()
	install(TARGETS lwp)
	install(DIRECTORY wallpapers
			DESTINATION share/lwp)
	install(FILES LICENSE.txt
			DESTINATION share/lwp)
	install(FILES ${_DEFAULT_CONFIG_FILE}
			TYPE SYSCONF
			RENAME lwp.cfg)
endif()

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

# Installer
set(CPACK_GENERATOR WIX)
set(CPACK_WIX_UPGRADE_GUID "1cb3edc1-1059-4b20-b87c-90186a76a713")
set(CPACK_WIX_PRODUCT_ICON  ${CMAKE_CURRENT_SOURCE_DIR}/icon.ico)
set(CPACK_WIX_UI_BANNER  ${CMAKE_CURRENT_SOURCE_DIR}/installer/banner.png)
set(CPACK_WIX_UI_DIALOG  ${CMAKE_CURRENT_SOURCE_DIR}/installer/dialog.png)
set(CPACK_PACKAGE_NAME "Layered WallPaper")
set(CPACK_PACKAGE_VENDOR "Jakub Szczerbinski")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Multi-platform parallax wallpapers")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Layered WallPaper")

INCLUDE(CPack)