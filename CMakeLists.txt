cmake_minimum_required(VERSION 3.13)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(lwp
    LANGUAGES C)

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

# Config file
set(_DEFAULT_CONFIG_FILE default.cfg)
if(_UNAME STREQUAL "WIN32")
	set(_DEFAULT_CONFIG_FILE defaultWin.cfg)
endif()
if (_UNAME STREQUAL "DARWIN")
  set(_DEFAULT_CONFIG_FILE defaultMac.cfg)
endif()

add_subdirectory(src/wlp)
link_directories(src/wlp) 

add_subdirectory(src/core)
link_directories(src/core) 

# Installation rules
if(_UNAME STREQUAL "WIN32")
	install(DIRECTORY wallpapers
			DESTINATION .)
	install(DIRECTORY src/window_templates
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
	install(DIRECTORY wallpapers
			DESTINATION usr/local/share/lwp)
	install(DIRECTORY src/window_templates
			DESTINATION usr/local/share/lwp)
	install(FILES LICENSE.txt
			DESTINATION usr/local/share/lwp)
	install(FILES ${_DEFAULT_CONFIG_FILE}
			TYPE SYSCONF
			RENAME lwp.cfg)
else()
        install(DIRECTORY wallpapers
                        DESTINATION Layered_WallPaper)
		install(DIRECTORY src/window_templates
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
