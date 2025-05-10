cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(jpaper)

# ===========================================
#               Detect platform
# ===========================================
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(TARGET_OS "LINUX")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(TARGET_OS "WINDOWS")
else()
	set(TARGET_OS "UNSUPPORTED")
endif()

message("Target platform: ${TARGET_OS}")

# ===========================================
#               Detect version
# ===========================================
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
  message(WARNING "Can't determine jpaper version")
endif()


set(SOURCE_FILES
	src/main.c
	src/log.c
	src/assets.c
	src/utils.c
	src/gui/gui.c
	src/gui/wnd_main.c
	src/gui/wnd_wlp.c
	src/wallpaper/
	src/wallpaper/wlp_loader.c
	src/wallpaper/wlp_instance.c
	src/wallpaper/wlp_config.c
	src/wallpaper/layer/anchor.c
	src/wallpaper/layer/behaviour.c
	src/wallpaper/layer/layer.c
	src/wallpaper/layer/layer_anim.c
	src/wallpaper/layer/layer_col.c
	src/wallpaper/layer/layer_img.c
	src/wallpaper/layer/layer_text.c
)

configure_file(${CMAKE_SOURCE_DIR}/schemas/wallpaper.xsd ${CMAKE_SOURCE_DIR}/build/schemas/wallpaper.xsd COPYONLY)

if(TARGET_OS STREQUAL "LINUX")
	configure_file(${CMAKE_SOURCE_DIR}/src/gui/jpaper.ui ${CMAKE_SOURCE_DIR}/build/share/jpaper/jpaper.ui COPYONLY)
	configure_file(${CMAKE_SOURCE_DIR}/src/gui/style.css ${CMAKE_SOURCE_DIR}/build/share/jpaper/style.css COPYONLY)
elseif(TARGET_OS STREQUAL "WINDOWS")
	configure_file(${CMAKE_SOURCE_DIR}/src/gui/jpaper.ui ${CMAKE_SOURCE_DIR}/build/jpaper.ui COPYONLY)
	configure_file(${CMAKE_SOURCE_DIR}/src/gui/style.css ${CMAKE_SOURCE_DIR}/build/style.css COPYONLY)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

if(TARGET_OS STREQUAL "LINUX")
	list(APPEND SOURCE_FILES "src/platform/linux.c")
elseif(TARGET_OS STREQUAL "WINDOWS")
	list(APPEND SOURCE_FILES "src/platform/windows.c")
endif()

find_package (PkgConfig REQUIRED)

# libxml2 dependency
find_package(libxml2 REQUIRED)
list(APPEND _INCLUDE_DIRS ${LIBXML2_INCLUDE_DIRS})
list(APPEND _LIBS ${LIBXML2_LIBRARIES})

# SDL3 dependency
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
list(APPEND _LIBS SDL3::SDL3-shared)
find_package(SDL3_ttf REQUIRED CONFIG)
list(APPEND _LIBS SDL3_ttf::SDL3_ttf)

# GTK4 dependency
pkg_check_modules (GTK4 REQUIRED gtk4)
list(APPEND _INCLUDE_DIRS ${GTK4_INCLUDE_DIRS})
list(APPEND _LIBS ${GTK4_LINK_LIBRARIES})

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")
endif()

add_executable(jpaper ${SOURCE_FILES})

if (TARGET_OS STREQUAL "LINUX")
  # X11 and Xrandr dependency
  include(FindX11)

  list(APPEND _INCLUDE_DIRS ${X11_INCLUDE_DIR})
  list(APPEND _LIBS ${X11_LIBRARIES})

  list(APPEND _INCLUDE_DIRS ${X11_Xrandr_INCLUDE_PATH})
  list(APPEND _LIBS ${X11_Xrandr_LIB})
elseif(TARGET_OS STREQUAL "WINDOWS")
	# DLLs
	add_custom_command(POST_BUILD TARGET jpaper WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND sh deploy_dlls.sh mingw64 ${CMAKE_SOURCE_DIR}/build)
	# Shcore dependency
	list(APPEND _LIBS "-lShcore")
	add_definitions(-D_WIN32_WINNT=0x0A00)
endif()

target_compile_definitions(jpaper PUBLIC OS_${TARGET_OS} OS_NAME="${TARGET_OS}" PROGRAM_VERSION="${PROGRAM_VERSION}")
target_include_directories(jpaper PUBLIC ${_INCLUDE_DIRS})
target_link_libraries(jpaper PRIVATE ${_LIBS})

