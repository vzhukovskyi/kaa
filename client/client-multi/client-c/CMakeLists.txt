#
# Copyright 2014 CyberVision, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 2.8.8)

project (Kaa-c)
enable_language(C)

# Path to CMake's scripts.
set(CMAKE_MODULE_PATH 
        ${CMAKE_MODULE_PATH} 
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules
    )

#
# Sets common compilation flags.
# 
# NOTE: Platform specific compilation flags should be set 
# in the corresponding platform CMake script(s).
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall")

# Compiler specific flags.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
endif()

if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

    if(KAA_DEBUG_ENABLED)
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -ggdb")
    else()
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os -s")
    endif()

    if(KAA_COLLECT_COVERAGE AND KAA_DEBUG_ENABLED)
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} --coverage")
    endif()
endif()

# Creates variable to specify thirdparty libraries.
set(KAA_THIRDPARTY_LIBRARIES "")

# Release version is built by default.
if(KAA_DEBUG_ENABLED)
    set(CMAKE_BUILD_TYPE DEBUG)
else()
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

# Sets the default build platform to POSIX.
if(NOT DEFINED KAA_PLATFORM)
  set(KAA_PLATFORM "x86-64")
endif()

#
# Sets maximum Kaa SDK log level.
#
# Logging constants:
# NONE  - 0
# FATAL - 1
# ERROR - 2
# WARN  - 3
# INFO  - 4
# DEBUG - 5
# TRACE - 6
if(NOT DEFINED KAA_MAX_LOG_LEVEL)
    if(KAA_DEBUG_ENABLED)
        set(KAA_MAX_LOG_LEVEL 6)
    else()
        set(KAA_MAX_LOG_LEVEL 4)
    endif()
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DKAA_MAX_LOG_LEVEL=${KAA_MAX_LOG_LEVEL}")

# Disables Kaa library modules.
message("==================================")
if(KAA_WITHOUT_EVENTS)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DKAA_DISABLE_FEATURE_EVENTS")
else()
    message( "EVENTS ENABLED")
endif()

if(KAA_WITHOUT_LOGGING)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DKAA_DISABLE_FEATURE_LOGGING")
else()
     message( "LOGGING ENABLED")
endif()

if(KAA_WITHOUT_CONFIGURATION)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DKAA_DISABLE_FEATURE_CONFIGURATION")
else()
     message("CONFIGURATION ENABLED")
endif()
message("==================================")
# Sets path(s) to header files.
set (KAA_SRC_FOLDER "src/kaa")
include_directories(KAA_INCLUDE_PATHS
                        ${KAA_INCLUDE_PATHS}
                        ${CMAKE_CURRENT_SOURCE_DIR}/${KAA_SRC_FOLDER}
                   )

# Sets path(s) to source files.
set (KAA_SOURCE_FILES
        ${KAA_SOURCE_FILES}
        ${KAA_SRC_FOLDER}/avro_src/io.c
        ${KAA_SRC_FOLDER}/avro_src/encoding_binary.c
        ${KAA_SRC_FOLDER}/collections/kaa_deque.c
        ${KAA_SRC_FOLDER}/collections/kaa_list.c
        ${KAA_SRC_FOLDER}/utilities/kaa_log.c
        ${KAA_SRC_FOLDER}/utilities/kaa_mem.c
        ${KAA_SRC_FOLDER}/utilities/kaa_buffer.c
        ${KAA_SRC_FOLDER}/kaa_platform_utils.c
        ${KAA_SRC_FOLDER}/kaa_platform_protocol.c
        ${KAA_SRC_FOLDER}/kaa_bootstrap_manager.c
        ${KAA_SRC_FOLDER}/kaa_channel_manager.c
        ${KAA_SRC_FOLDER}/kaa_common_schema.c
        ${KAA_SRC_FOLDER}/kaa_event.c
        ${KAA_SRC_FOLDER}/kaa_logging.c
        ${KAA_SRC_FOLDER}/kaa_profile.c
        ${KAA_SRC_FOLDER}/kaa_configuration_manager.c
        ${KAA_SRC_FOLDER}/kaa_status.c
        ${KAA_SRC_FOLDER}/kaa_user.c
        ${KAA_SRC_FOLDER}/kaa.c
    )

# Includes auto-generated and platform-dependent Cmake's scripts.
include(${CMAKE_CURRENT_SOURCE_DIR}/listfiles/CMakeGen.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/listfiles/platform/${KAA_PLATFORM}/CMakeLists.txt)

# Prints build parameters.
message( "BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message( "KAA_PLATFORM = ${KAA_PLATFORM}")
message( "KAA_MAX_LOG_LEVEL = ${KAA_MAX_LOG_LEVEL}")

# Builds Kaa libraries.
add_library(kaac_o OBJECT ${KAA_SOURCE_FILES})
add_library(kaac_s STATIC ./${KAA_SRC_FOLDER} $<TARGET_OBJECTS:kaac_o>)
add_library(kaac SHARED ./${KAA_SRC_FOLDER} $<TARGET_OBJECTS:kaac_o>)
target_link_libraries(kaac ${KAA_THIRDPARTY_LIBRARIES})

# Sets paths Kaa libraries will be installed in.
if(NOT KAA_INSTALL_PATH)
	set(KAA_INSTALL_PATH "/usr/local")
endif()

message("KAA WILL BE INSTALLED TO  ${KAA_INSTALL_PATH} ")
set(CMAKE_INSTALL_PREFIX ${KAA_INSTALL_PATH} ) 
install (DIRECTORY ${KAA_SRC_FOLDER}/ DESTINATION ${KAA_INSTALL_PATH}/include/kaa
    FILES_MATCHING PATTERN *.h)
install (TARGETS kaac kaac_s DESTINATION ${KAA_INSTALL_PATH}/lib)


# Builds unit tests.
if(KAA_UNITTESTS_COMPILE)
    include(${CMAKE_CURRENT_SOURCE_DIR}/listfiles/UnitTest.cmake)
endif()
