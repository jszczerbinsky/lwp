cmake_minimum_required(VERSION 3.13)

project(lwp
    LANGUAGES C)

if(SDL2_RUNTIME_DIR)
	string(REPLACE "\\" "/" SDL2_RUNTIME_DIR ${SDL2_RUNTIME_DIR})
endif()

# Detect the platform
if (APPLE)
  set(_UNAME "DARWIN")
elseif (WIN32)
  set(_UNAME "WIN32")
else()
  set(_UNAME "LINUX")
endif()

# Detect version
find_package(Git)
if(Git_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --dirty --match "v*"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_DESCRIBE_VERSION
    RESULT_VARIABLE GIT_DESCRIBE_ERROR_CODE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(NOT GIT_DESCRIBE_ERROR_CODE)
    set(PROGRAM_VERSION ${GIT_DESCRIBE_VERSION})
  endif()
endif()
if(NOT DEFINED PROGRAM_VERSION)
  set(PROGRAM_VERSION v0.0.0-unknown)
  message(WARNING "Can't determine Layered WallPaper version")
endif()

set(_SOURCE_FILES
    main.c
		debug.c
    parser.c
    wallpaper.c
    window.c
    )
		
# Windows resource file
if(_UNAME STREQUAL "WIN32")
	if(MINGW)
		set(CMAKE_RC_COMPILER_INIT windres)
		ENABLE_LANGUAGE(RC)
		SET(CMAKE_RC_COMPILE_OBJECT
    "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
	endif(MINGW)
	
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/resource.template.rc ${CMAKE_CURRENT_SOURCE_DIR}/resource.rc
        @ONLY)
	list(APPEND _SOURCE_FILES "resource.rc")
endif()

# SDL2 dependency
find_package(SDL2 REQUIRED CONFIG)
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

if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
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
	install(FILES ${SDL2_RUNTIME_DIR}/SDL2.dll
			DESTINATION .)
	install(FILES ${SDL2_RUNTIME_DIR}/README-SDL.txt
			DESTINATION .)
else()
	install(TARGETS lwp
			DESTINATION usr/local/bin)
	install(DIRECTORY wallpapers
			DESTINATION usr/local/share/lwp)
	install(FILES LICENSE.txt
			DESTINATION usr/local/share/lwp)
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
if (_UNAME STREQUAL "WIN32")
	set(CPACK_GENERATOR NSIS)
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "Layered WallPaper")
elseif(_UNAME STREQUAL "LINUX")
	set(CPACK_GENERATOR TGZ)
	set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
endif()
set(CPACK_NSIS_MUI_ICON  ${CMAKE_CURRENT_SOURCE_DIR}/icon.ico)
set(CPACK_NSIS_MUI_UNIICON  ${CMAKE_CURRENT_SOURCE_DIR}/icon.ico)
set(CPACK_NSIS_INSTALLED_ICON_NAME  ${CMAKE_CURRENT_SOURCE_DIR}/icon.ico)
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
  WriteRegExpandStr HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Run' 'lwp'	'\$INSTDIR\\\\lwp.exe'
	Exec '\$INSTDIR\\\\lwp.exe'
   ")
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
  DeleteRegValue HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Run' 'lwp'
 ")
set(CPACK_PACKAGE_NAME "Layered WallPaper")
set(CPACK_PACKAGE_VENDOR "Jakub Szczerbinski")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Multi-platform parallax wallpapers")
set(CPACK_PACKAGE_VERSION ${PROGRAM_VERSION})

INCLUDE(CPack)
