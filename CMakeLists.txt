cmake_minimum_required(VERSION 3.3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(JPaper)

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
  message(WARNING "Can't determine JPaper version")
endif()


set(SOURCE_FILES
	src/main.c
	src/wlp.c
	src/wlpconfig.c
	src/log.c
	src/assets.c
	src/utils.c
	src/layer/anchor.c
	src/layer/behaviour.c
	src/layer/layer.c
	src/layer/layer_anim.c
	src/layer/layer_col.c
	src/layer/layer_img.c
	src/layer/layer_text.c
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

if(TARGET_OS STREQUAL "LINUX")
	list(APPEND SOURCE_FILES "src/platform/linux.c")
elseif(TARGET_OS STREQUAL "WINDOWS")
	list(APPEND SOURCE_FILES "src/platform/windows.c")
endif()

#libxml2 dependency
find_package(libxml2 REQUIRED)
list(APPEND _INCLUDE_DIRS ${LIBXML2_INCLUDE_DIRS})
list(APPEND _LIBS ${LIBXML2_LIBRARIES})

# SDL3 dependency
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
list(APPEND _LIBS SDL3::SDL3-shared)
find_package(SDL3_ttf REQUIRED CONFIG)
list(APPEND _LIBS SDL3_ttf::SDL3_ttf)

if (TARGET_OS STREQUAL "LINUX")
  # X11 and Xrandr dependency
  include(FindX11)

  list(APPEND _INCLUDE_DIRS ${X11_INCLUDE_DIR})
  list(APPEND _LIBS ${X11_LIBRARIES})

  list(APPEND _INCLUDE_DIRS ${X11_Xrandr_INCLUDE_PATH})
  list(APPEND _LIBS ${X11_Xrandr_LIB})
endif()

add_executable(JPaper ${SOURCE_FILES})

target_compile_definitions(JPaper PUBLIC OS_${TARGET_OS} OS_NAME="${TARGET_OS}" PROGRAM_VERSION="${PROGRAM_VERSION}")
target_include_directories(JPaper PUBLIC ${_INCLUDE_DIRS})
target_link_libraries(JPaper PRIVATE ${_LIBS})
