cmake_minimum_required(VERSION 3.13)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(lwp
    LANGUAGES C)

# Detect the platform
if (MSYS OR MINGW)
    set(_UNAME "WIN32")
elseif(UNIX)
    set(_UNAME "LINUX")
else()
	message(FATAL_ERROR "Unsupported platform")
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

add_subdirectory(src/wlp)
link_directories(src/wlp) 

add_subdirectory(src/core)
link_directories(src/core)

# Installation rules
if(_UNAME STREQUAL "WIN32")
	install(DIRECTORY wallpapers
			DESTINATION .)
	install(DIRECTORY build/dlls/ 
			DESTINATION .)
	install(DIRECTORY src/window_templates
			DESTINATION .)
	install(FILES LICENSE.txt
			DESTINATION .)
	install(FILES ${SDL2_RUNTIME_DIR}/SDL2.dll
			DESTINATION .)
	install(FILES ${SDL2_RUNTIME_DIR}/README-SDL.txt
			DESTINATION .)
else()
	install(DIRECTORY wallpapers
			DESTINATION share/lwp)
	install(DIRECTORY src/window_templates
			DESTINATION share/lwp)
	install(FILES LICENSE.txt
			DESTINATION share/lwp)
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
	CreateShortCut \\\"$DESKTOP\\\\Layered WallPaper.lnk\\\" \\\"$INSTDIR\\\\lwp.exe\\\"
   ")
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
  DeleteRegValue HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Run' 'lwp'
  Delete \\\"$DESKTOP\\\\Layered WallPaper.lnk\\\"
 ")
set(CPACK_PACKAGE_NAME "Layered WallPaper")
set(CPACK_PACKAGE_VENDOR "Jakub Szczerbinski")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Multi-platform parallax wallpapers")
set(CPACK_PACKAGE_VERSION ${PROGRAM_VERSION})

INCLUDE(CPack)
