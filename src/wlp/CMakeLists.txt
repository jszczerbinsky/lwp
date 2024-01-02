if(SDL2_RUNTIME_DIR)
	string(REPLACE "\\" "/" SDL2_RUNTIME_DIR ${SDL2_RUNTIME_DIR})
endif()

set(_SOURCE_FILES
    main.c
	debug.c
    parser.c
    wallpaper.c
    window.c
    )

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

# Main executable
if (_UNAME STREQUAL "DARWIN")
  add_executable(lwpwlp MACOSX_BUNDLE ${_SOURCE_FILES})
else()
  add_executable(lwpwlp ${_SOURCE_FILES})
endif()

# Windows specific properties for executable
if(_UNAME STREQUAL "WIN32")
	set_property(TARGET lwpwlp PROPERTY VS_DPI_AWARE "PerMonitor")
endif()

if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

target_compile_definitions(lwpwlp PUBLIC __${_UNAME})
target_include_directories(lwpwlp PUBLIC ${_INCLUDE_DIRS})
target_link_libraries(lwpwlp PRIVATE ${_LIBS})

# Installation rules
if(_UNAME STREQUAL "WIN32")
	install(TARGETS lwpwlp
			DESTINATION .)
elseif(_UNAME STREQUAL "LINUX")
	install(TARGETS lwpwlp
			DESTINATION usr/local/bin)
else()
	install(TARGETS lwpwlp 
			DESTINATION Layered_WallPaper)
endif()

