cmake_minimum_required (VERSION 3.6)
project (LED-CUBE-SIMULATOR-EXAMPLES)

set (SIMULATOR_VERSION_MAJOR 1)
set (SIMULATOR_VERSION_MINOR 0)
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_INSTALL_PREFIX ".")

if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE Release)
endif()

set (CMAKE_CXX_FLAGS "-Wall")
set (CMAKE_CXX_FLAGS_DEBUG "-g")
set (CMAKE_CXX_FLAGS_RELEASE "-O3")

find_library(SFML_NETWORK NAMES sfml-network-2 sfml-network PATHS ${SFML-DIR}/bin)
if (NOT SFML_NETWORK)
    message(FATAL_ERROR "Unable to find sfml-network library/framework, is sfml installed? Please see wiki for details.")
endif()

find_library(PNG NAMES png16 png PATHS /usr/local/libpng /usr/local/lib)
find_path(PNG_INCLUDE_DIR "png.hpp" HINT ${PNG} PATH /usr/local/libpng)
if (NOT PNG)
	message(WARNING "Unable to find libpng, skipping Ex_Animate.cpp")
else()
	add_executable(Ex_Animate Ex_Animate.cpp)
	target_include_directories(Ex_Animate PRIVATE ${PNG_INCLUDE_DIR})
	target_link_libraries(Ex_Animate ${SFML_NETWORK} ${PNG} z)
endif()

add_executable(Ex_Circles Ex_Circles.cpp)
target_link_libraries(Ex_Circles ${SFML_NETWORK})

add_executable(Ex_Columns Ex_Columns.cpp)
target_link_libraries(Ex_Columns ${SFML_NETWORK})

add_executable(Ex_Corners Ex_Corners.cpp)
target_link_libraries(Ex_Corners ${SFML_NETWORK})

add_executable(Ex_Faces Ex_Faces.cpp)
target_link_libraries(Ex_Faces ${SFML_NETWORK})

add_executable(Ex_Rows Ex_Rows.cpp)
target_link_libraries(Ex_Rows ${SFML_NETWORK})

install (TARGETS Ex_Animate Ex_Circles Ex_Columns Ex_Corners Ex_Faces Ex_Rows DESTINATION "../")
