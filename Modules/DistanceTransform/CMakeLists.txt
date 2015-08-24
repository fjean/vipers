CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(DISTTRANSFORM_MODULE)

# This is the package release versioning info
SET(DISTTRANSFORM_MODULE_RELEASE_VERSION 0.1.0)

# Find VIPERS library
IF(NOT VIPERS_FOUND)
  FIND_PACKAGE(VIPERS REQUIRED)
ENDIF()
INCLUDE_DIRECTORIES( ${VIPERS_INCLUDE_DIR} )

# Find OpenCV
IF(NOT OpenCV_INCLUDE_DIRS)
  FIND_PACKAGE(OpenCV REQUIRED CV CXCORE HIGHGUI)
ENDIF()
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )

# Generate version.hpp file
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/DistanceTransformConfig.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/DistanceTransformConfig.hpp)

# Glob all sources and headers
FILE(GLOB DistTransformModuleSources *.cpp)
FILE(GLOB DistTransformModuleHeaders *.hpp)

# Create target
ADD_LIBRARY (distancetransform MODULE ${DistTransformModuleSources})
SET_TARGET_PROPERTIES(distancetransform PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(distancetransform ${VIPERS_LIBRARIES} ${OpenCV_LIBRARIES})

# Install
IF(VIPERSALL_BUNDLE)
    INSTALL(TARGETS distancetransform DESTINATION bin/modules)
ELSE()
    INSTALL(TARGETS distancetransform DESTINATION share/VIPERS/modules)
ENDIF()
