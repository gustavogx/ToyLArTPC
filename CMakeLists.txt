#---------------------------------------------------------------------
# ToyLArTPC - Geant4 Simulation of a Simplified LArTPC
#---------------------------------------------------------------------
cmake_minimum_required(VERSION 3.16...3.28)
project(ToyLArTPC VERSION 0.1.0 LANGUAGES CXX)

#---------------------------------------------------------------------
# Find Geant4 (require Qt visualization support)
#---------------------------------------------------------------------
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

#---------------------------------------------------------------------
# Find ROOT (needed to read pre-generated events and for output)
#---------------------------------------------------------------------
find_package(ROOT REQUIRED COMPONENTS Core RIO Tree)
include(${ROOT_USE_FILE})

#---------------------------------------------------------------------
# Find PNG (needed for image generation)
#---------------------------------------------------------------------
find_package(PNG REQUIRED)

#---------------------------------------------------------------------
# MARLEY neutrino event generator (only for standalone generator)
#---------------------------------------------------------------------
set(MARLEY_DIR $ENV{HOME}/Scientific/marley)
set(MARLEY_INCLUDE_DIR ${MARLEY_DIR}/include)
set(MARLEY_LIB ${MARLEY_DIR}/build/libMARLEY.so)

#---------------------------------------------------------------------
# Project sources (main simulation – no MARLEY dependency)
#---------------------------------------------------------------------
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(ToyLArTPC main.cc ${SOURCES} ${HEADERS})
target_include_directories(ToyLArTPC PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(ToyLArTPC ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#---------------------------------------------------------------------
# Standalone MARLEY event generator (links MARLEY + ROOT, no Geant4)
#---------------------------------------------------------------------
add_executable(GenerateMarleyEvents generate_marley_events.cc)
target_include_directories(GenerateMarleyEvents PRIVATE
    ${MARLEY_INCLUDE_DIR}
)
target_link_libraries(GenerateMarleyEvents ${MARLEY_LIB} ${ROOT_LIBRARIES})

#---------------------------------------------------------------------
# Image generator from ROOT files (reads PhotonCounts tree)
#---------------------------------------------------------------------
add_executable(GenerateImages generate_images.cc)
target_include_directories(GenerateImages PRIVATE
    ${ROOT_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
)
target_link_libraries(GenerateImages ${ROOT_LIBRARIES} ${PNG_LIBRARIES})

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

