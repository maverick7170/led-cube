cmake_minimum_required (VERSION 3.6)
project (LED_CUBE_SIMULATOR_EXAMPLES)

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

find_library(SFML_NETWORK NAMES sfml-network-2 sfml-network PATHS ${SFML_DIR}/bin)
if (NOT SFML_NETWORK)
    message(FATAL_ERROR "Unable to find sfml-network library/framework, is sfml installed? Please see wiki for details.")
endif()

find_path(SFML_INCLUDES "SFML/Network.hpp" HINT "${SFML_NETWORK}//..//include" PATHS ${SFML_DIR}/include)
if (NOT SFML_INCLUDES)
	message(FATAL_ERROR "SFML seems to be installed but cannot located the included header files. Please set the SFML-DIR variable when calling cmake. For example, cmake -DSFML-DIR=\"/usr/local/sfml\"")
endif()

find_library(PNG16 NAMES png16-16 png16 png PATHS /usr/local/libpng /usr/local/lib ${PNG_LIB})
find_path(PNG_INCLUDE_DIR "png.hpp" HINT ${PNG_LIB} PATH ${PNG_LIB} /usr/local/libpng)
if (NOT PNG16)
	message(WARNING "Unable to find libpng, skipping Ex_Animate.cpp. Please use cmake -DPNG_LIB=C:\\path_to_png")
else()
	add_executable(Ex_Animate Ex_Animate.cpp)
	target_include_directories(Ex_Animate PRIVATE ${SFML_INCLUDES} ${PNG_INCLUDE_DIR})
	target_link_libraries(Ex_Animate ${SFML_NETWORK} ${PNG16} z)
	install (TARGETS Ex_Animate DESTINATION "../")
endif()

add_executable(Ex_Circles Ex_Circles.cpp)
target_include_directories(Ex_Circles PRIVATE ${SFML_INCLUDES})
target_link_libraries(Ex_Circles ${SFML_NETWORK})

add_executable(Ex_Columns Ex_Columns.cpp)
target_include_directories(Ex_Columns PRIVATE ${SFML_INCLUDES})
target_link_libraries(Ex_Columns ${SFML_NETWORK})

add_executable(Ex_Corners Ex_Corners.cpp)
target_include_directories(Ex_Corners PRIVATE ${SFML_INCLUDES})
target_link_libraries(Ex_Corners ${SFML_NETWORK})

add_executable(Ex_Equalizer Ex_Equalizer.cpp)
target_include_directories(Ex_Equalizer PRIVATE ${SFML_INCLUDES})
target_link_libraries(Ex_Equalizer ${SFML_NETWORK})

add_executable(Ex_Faces Ex_Faces.cpp)
target_include_directories(Ex_Faces PRIVATE ${SFML_INCLUDES})
target_link_libraries(Ex_Faces ${SFML_NETWORK})

add_executable(Ex_Rows Ex_Rows.cpp)
target_include_directories(Ex_Rows PRIVATE ${SFML_INCLUDES})
target_link_libraries(Ex_Rows ${SFML_NETWORK})

install (TARGETS Ex_Circles Ex_Columns Ex_Corners Ex_Equalizer Ex_Faces Ex_Rows DESTINATION "../")
