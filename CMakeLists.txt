#---------------------------------------------------------------------
# ToyLArTPC - Geant4 Simulation of a Simplified LArTPC
#---------------------------------------------------------------------
cmake_minimum_required(VERSION 3.16...3.28)
project(ToyLArTPC VERSION 0.1.0 LANGUAGES CXX)

#---------------------------------------------------------------------
# Set C++ standard to C++20
#---------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#---------------------------------------------------------------------
# Find Geant4 (require Qt visualization support)
#---------------------------------------------------------------------
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

#---------------------------------------------------------------------
# Find ROOT using root-config for portability
#---------------------------------------------------------------------
find_program(ROOT_CONFIG root-config)
if(ROOT_CONFIG)
    execute_process(
        COMMAND ${ROOT_CONFIG} --cflags
        OUTPUT_VARIABLE ROOT_CFLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${ROOT_CONFIG} --libs
        OUTPUT_VARIABLE ROOT_LIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    # Extract include directory (ROOT include dir is the -I flag)
    string(REGEX MATCHALL "-I[^ ]+" ROOT_INCLUDE_FLAGS "${ROOT_CFLAGS}")
    list(GET ROOT_INCLUDE_FLAGS 0 ROOT_INCLUDE_FLAG)
    string(REPLACE "-I" "" ROOT_INCLUDE_DIR "${ROOT_INCLUDE_FLAG}")
else()
    message(FATAL_ERROR "root-config not found. Please ensure ROOT is installed and in your PATH.")
endif()

#---------------------------------------------------------------------
# Find PNG (needed for image generation)
#---------------------------------------------------------------------
find_package(PNG REQUIRED)

#---------------------------------------------------------------------
# MARLEY neutrino event generator (only for standalone generator)
#---------------------------------------------------------------------
find_program(MARLEY_CONFIG marley-config)
if(MARLEY_CONFIG)
    execute_process(
        COMMAND ${MARLEY_CONFIG} --cflags
        OUTPUT_VARIABLE MARLEY_CFLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${MARLEY_CONFIG} --libs
        OUTPUT_VARIABLE MARLEY_LIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    # Extract include directory (last -I flag is the marley include dir)
    string(REGEX MATCHALL "-I[^ ]+" MARLEY_INCLUDE_FLAGS "${MARLEY_CFLAGS}")
    list(GET MARLEY_INCLUDE_FLAGS -1 MARLEY_INCLUDE_FLAG)
    string(REPLACE "-I" "" MARLEY_INCLUDE_DIR "${MARLEY_INCLUDE_FLAG}")
else()
    message(FATAL_ERROR "marley-config not found. Please ensure MARLEY is installed and in your PATH.")
endif()

#---------------------------------------------------------------------
# Project sources (main simulation – no MARLEY dependency)
#---------------------------------------------------------------------
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(ToyLArTPC main.cc ${SOURCES} ${HEADERS})
target_include_directories(ToyLArTPC PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${Geant4_INCLUDE_DIRS}
    ${ROOT_INCLUDE_DIR}
)
target_compile_definitions(ToyLArTPC PRIVATE ${Geant4_DEFINITIONS})
target_link_libraries(ToyLArTPC PRIVATE ${Geant4_LIBRARIES} ${ROOT_LIBS})

#---------------------------------------------------------------------
# Standalone MARLEY event generator (links MARLEY + ROOT, no Geant4)
#---------------------------------------------------------------------
add_executable(GenerateMarleyEvents generate_marley_events.cc)
target_include_directories(GenerateMarleyEvents PRIVATE
    ${MARLEY_INCLUDE_DIR}
    ${ROOT_INCLUDE_DIR}
)
target_link_libraries(GenerateMarleyEvents ${MARLEY_LIBS} ${ROOT_LIBS})

#---------------------------------------------------------------------
# Image generator from ROOT files (reads PhotonCounts tree)
#---------------------------------------------------------------------
add_executable(GenerateImages generate_images.cc)
target_include_directories(GenerateImages PRIVATE
    ${ROOT_INCLUDE_DIR}
    ${PNG_INCLUDE_DIRS}
)
target_link_libraries(GenerateImages ${ROOT_LIBS} ${PNG_LIBRARIES})

#---------------------------------------------------------------------
# Copy macro files and MARLEY config to build directory
#---------------------------------------------------------------------
file(GLOB MACRO_FILES ${PROJECT_SOURCE_DIR}/macros/*.mac)
foreach(_macro ${MACRO_FILES})
    get_filename_component(_macro_name ${_macro} NAME)
    configure_file(${_macro} ${PROJECT_BINARY_DIR}/${_macro_name} COPYONLY)
endforeach()

configure_file(${PROJECT_SOURCE_DIR}/marley_config.js
    ${PROJECT_BINARY_DIR}/marley_config.js COPYONLY)

