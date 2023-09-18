cmake_minimum_required(VERSION 3.13)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

set(_CORE_SOURCE_FILES
    core/main.c
		core/debug.c
    core/parser.c
    core/wallpaper.c
    core/window.c
    )

set(_CONTROL_SOURCE_FILES
	control_panel/main.c
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
	list(APPEND _CORE_SOURCE_FILES "resource.rc")
endif()

# SDL2 dependency
find_package(SDL2 REQUIRED CONFIG)
set(_CORE_INCLUDE_DIRS ${SDL2_CORE_INCLUDE_DIRS})
set(_CORE_LIBS ${SDL2_LIBRARIES})

# GTK4 depenency
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
set(_CONTROL_LIBS PkgConfig::GTK4)

if (_UNAME STREQUAL "DARWIN")
  # MacOSX framework dependencies
  list(APPEND _CORE_LIBS "-framework CoreGraphics" "-framework Foundation")
endif()

if (_UNAME STREQUAL "LINUX")
  # X11 dependency
  find_package(X11 REQUIRED)
  list(APPEND _CORE_INCLUDE_DIRS ${X11_INCLUDE_DIR})
  list(APPEND _CORE_LIBS ${X11_LIBRARIES})
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

# Core executable
if (_UNAME STREQUAL "DARWIN")
  add_executable(lwpwlp MACOSX_BUNDLE ${_CORE_SOURCE_FILES})
else()
  add_executable(lwpwlp ${_CORE_SOURCE_FILES})
endif()

# Control executable
add_executable(lwp ${_CONTROL_SOURCE_FILES})

# Windows specific properties for executable
if(_UNAME STREQUAL "WIN32")
	set_property(TARGET lwp PROPERTY VS_DPI_AWARE "PerMonitor")
endif()

if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

target_compile_definitions(lwpwlp PUBLIC __${_UNAME} VER="${PROGRAM_VERSION}")
target_include_directories(lwpwlp PUBLIC ${_CORE_INCLUDE_DIRS})
target_link_libraries(lwpwlp PRIVATE ${_CORE_LIBS})

target_compile_definitions(lwp PUBLIC __${_UNAME} VER="${PROGRAM_VERSION}")
target_link_libraries(lwp PRIVATE ${_CONTROL_LIBS})

# Installation rules
if(_UNAME STREQUAL "WIN32")
	install(TARGETS lwp
			DESTINATION .)
	install(TARGETS lwpwlp
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
elseif(_UNAME STREQUAL "LINUX")
	install(TARGETS lwp
			DESTINATION usr/local/bin)
	install(TARGETS lwpwlp
			DESTINATION usr/local/bin)
	install(DIRECTORY wallpapers
			DESTINATION usr/local/share/lwp)
	install(FILES LICENSE.txt
			DESTINATION usr/local/share/lwp)
	install(FILES ${_DEFAULT_CONFIG_FILE}
			TYPE SYSCONF
			RENAME lwp.cfg)
else()
	install(TARGETS lwp
                        DESTINATION Layered_WallPaper)
				install(TARGETS lwpwlp
												DESTINATION Layered_WallPaper)
        install(DIRECTORY wallpapers
                        DESTINATION Layered_WallPaper)
        install(FILES LICENSE.txt
                        DESTINATION Layered_WallPaper)
        install(FILES ${_DEFAULT_CONFIG_FILE}
			DESTINATION Layered_WallPaper 
                        RENAME lwp.cfg)
	install(FILES lwp.template.plist
			DESTINATION Layered_WallPaper)
	install (FILES setupPlist.command
			DESTINATION Layered_WallPaper
			RENAME Toggle_Autorun.command
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	install (FILES setupPlist.command
			DESTINATION .
			RENAME Toggle_Autorun.command
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

endif()

# Installer
if (_UNAME STREQUAL "WIN32")
	set(CPACK_GENERATOR NSIS)
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "Layered WallPaper")
elseif(_UNAME STREQUAL "LINUX")
	set(CPACK_GENERATOR TGZ)
	set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
elseif(_UNAME STREQUAL "DARWIN")
	set(CPACK_GENERATOR DragNDrop)
	set(CPACK_DMG_BACKGROUND_IMAGE ${CMAKE_CURRENT_SOURCE_DIR}/dmgBg.png)
	set(CPACK_DMG_DS_STORE_SETUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/setupDmg.applescript)
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
