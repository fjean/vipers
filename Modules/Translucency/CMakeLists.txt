CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(TRANSLUCENCY_MODULE)

# This is the package release versioning info
SET(TRANSLUCENCY_MODULE_RELEASE_VERSION 0.1.0)

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
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/TranslucencyConfig.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/TranslucencyConfig.hpp)

# Glob all sources and headers
FILE(GLOB TranslucencyModuleSources *.cpp)
FILE(GLOB TranslucencyModuleHeaders *.hpp)

# Create target
ADD_LIBRARY (translucency MODULE ${TranslucencyModuleSources})
SET_TARGET_PROPERTIES(translucency PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(translucency ${VIPERS_LIBRARIES} ${OpenCV_LIBRARIES})

# Install
IF(VIPERSALL_BUNDLE)
    INSTALL(TARGETS translucency DESTINATION bin/modules)
ELSE()
    INSTALL(TARGETS translucency DESTINATION share/VIPERS/modules)
ENDIF()
