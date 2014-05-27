cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED AVOCADO_CPP_CORE_DIRECTORY)
	message(STATUS "Avocado CPP core will use the ./core directory by default...")
	set(AVOCADO_CPP_CORE_DIRECTORY ./core)
endif()

if(NOT EXISTS ${AVOCADO_CPP_CORE_DIRECTORY})
	message(FATAL_ERROR "Avocado CPP core is not available!")
endif()

include(${AVOCADO_CPP_CORE_DIRECTORY}/CMakeLists-core.txt)

project(ScriptService-v8)

include_directories(deps/v8/include)

cmake_policy(SET CMP0015 NEW)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/deps)

add_subdirectory(deps)
add_subdirectory(Script)

add_dependencies(ScriptService-v8 ScriptService-v8-v8) 
