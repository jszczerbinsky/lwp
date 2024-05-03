if(SDL2_RUNTIME_DIR)
	string(REPLACE "\\" "/" SDL2_RUNTIME_DIR ${SDL2_RUNTIME_DIR})
endif()

set(_SOURCE_FILES
		../common/paths.c
		../common/config.c
		../common/monitorScanner.c
		../common/wallpaperScanner.c
    main.c
		debug.c
    render.c
    window.c
	)

# GLIB dependency
find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
set(_INCLUDE_DIRS ${GLIB_INCLUDE_DIRS})
set(_LIBS ${GLIB_LDFLAGS})

if (_UNAME STREQUAL "LINUX")
  # X11 and Xrandr dependency
  include(FindX11)

  list(APPEND _INCLUDE_DIRS ${X11_INCLUDE_DIR})
  list(APPEND _LIBS ${X11_LIBRARIES})

  list(APPEND _INCLUDE_DIRS ${X11_Xrandr_INCLUDE_PATH})
  list(APPEND _LIBS ${X11_Xrandr_LIB})
endif()

# libconfig dependency
pkg_check_modules (libconfig REQUIRED libconfig)
list(APPEND _INCLUDE_DIRS ${libconfig_INCLUDE_DIRS})
list(APPEND _LIBS ${libconfig_LINK_LIBRARIES})

# SDL2 dependency
find_package(SDL2 REQUIRED CONFIG)
list(APPEND _INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})
list(APPEND _LIBS ${SDL2_LIBRARIES})

if (_UNAME STREQUAL "LINUX")
  # X11 dependency
  find_package(X11 REQUIRED)
  list(APPEND _INCLUDE_DIRS ${X11_INCLUDE_DIR})
  list(APPEND _LIBS ${X11_LIBRARIES})
endif()

if (_UNAME STREQUAL "WIN32")
  # Shcore dependency
  list(APPEND _LIBS "-lShcore")
endif()

option(LWP_INSTALL_LAUNCHD "Launch lwp on login (MacOSX only)" OFF)

# Main executable
if(_UNAME STREQUAL "WIN32")
  add_executable(lwpwlp WIN32 ${_SOURCE_FILES})
else()
  add_executable(lwpwlp ${_SOURCE_FILES})
endif()

# Windows specific properties for executable
if(_UNAME STREQUAL "WIN32")
	set_property(TARGET lwpwlp PROPERTY VS_DPI_AWARE "PerMonitor")
endif()

target_compile_definitions(lwpwlp PUBLIC __${_UNAME})
target_include_directories(lwpwlp PUBLIC ${_INCLUDE_DIRS})
target_link_libraries(lwpwlp PRIVATE ${_LIBS})

# Installation rules
if(_UNAME STREQUAL "WIN32")
	install(TARGETS lwpwlp
			DESTINATION .)
else()
	install(TARGETS lwpwlp
			DESTINATION bin)
endif()

