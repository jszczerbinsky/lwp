set(_SOURCE_FILES
    main.c
		windowHandlers.c
		wlp.c
		../common/monitorScanner.c
		../common/config.c
		../common/wallpaperScanner.c
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

find_package (PkgConfig REQUIRED)

if (_UNAME STREQUAL "LINUX")
  # X11 and Xrandr dependency
  include(FindX11)

  list(APPEND _INCLUDE_DIRS ${X11_INCLUDE_DIR})
  list(APPEND _LIBS ${X11_LIBRARIES})

  list(APPEND _INCLUDE_DIRS ${X11_Xrandr_INCLUDE_PATH})
  list(APPEND _LIBS ${X11_Xrandr_LIB})
endif()

# GTK dependency
pkg_check_modules (GTK3 REQUIRED gtk+-3.0)
list(APPEND _INCLUDE_DIRS ${GTK3_INCLUDE_DIRS})
list(APPEND _LIBS ${GTK3_LINK_LIBRARIES})

# libconfig dependency
pkg_check_modules (libconfig REQUIRED libconfig)
list(APPEND _INCLUDE_DIRS ${libconfig_INCLUDE_DIRS})
list(APPEND _LIBS ${libconfig_LINK_LIBRARIES})

# Main executable
if (_UNAME STREQUAL "DARWIN")
  add_executable(lwp MACOSX_BUNDLE ${_SOURCE_FILES})
else()
  add_executable(lwp ${_SOURCE_FILES})
endif()

if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

set_target_properties(lwp PROPERTIES ENABLE_EXPORTS on)

target_compile_definitions(lwp PUBLIC __${_UNAME})
target_include_directories(lwp PUBLIC ${_INCLUDE_DIRS})
target_link_libraries(lwp PRIVATE ${_LIBS})

# Installation rules
if(_UNAME STREQUAL "WIN32")
	install(TARGETS lwp
			DESTINATION .)
elseif(_UNAME STREQUAL "LINUX")
	install(TARGETS lwp
			DESTINATION usr/local/bin)
else()
	install(TARGETS lwp 
			DESTINATION Layered_WallPaper)
endif()

