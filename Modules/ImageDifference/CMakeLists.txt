CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(IMAGEDIFFERENCE_MODULE)

# This is the package release versioning info
SET(IMAGEDIFFERENCE_MODULE_RELEASE_VERSION 0.1.0)

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
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/ImageDifferenceConfig.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/ImageDifferenceConfig.hpp)

# Glob all sources and headers
FILE(GLOB ImageDifferenceModuleSources *.cpp)
FILE(GLOB ImageDifferenceModuleHeaders *.hpp)

# Create target
ADD_LIBRARY (imagedifference MODULE ${ImageDifferenceModuleSources})
SET_TARGET_PROPERTIES(imagedifference PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(imagedifference ${VIPERS_LIBRARIES} ${OpenCV_LIBRARIES})

# Install
IF(VIPERSALL_BUNDLE)
    INSTALL(TARGETS imagedifference DESTINATION bin/modules)
ELSE()
    INSTALL(TARGETS imagedifference DESTINATION share/VIPERS/modules)
ENDIF()
