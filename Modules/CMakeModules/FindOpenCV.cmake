# - Try to find OpenCV library installation
# See http://sourceforge.net/projects/opencvlibrary/
#
# The OpenCV_ROOT_DIR environment variable is optionally searched for 
# OpenCV root directory, in case of a non standard installation
#
# Usage:
#  FIND_PACKAGE(OpenCV [REQUIRED | COMPONENTS] [COMPONENT1 COMPONENT2 ...])
#
#  Possible components are: 
#   CV CXCORE CVAUX HIGHGUI ML CVCAM
#   (note that CVCAM is only disponible on Windows)
#
#  Examples:
#   Use default components
#    FIND_PACKAGE( OpenCV )
#   Use CV CXCORE CVAUX and HIGHGUI
#    FIND_PACKAGE( OpenCV COMPONENTS CV CXCORE CVAUX HIGHGUI )
#
#  The following are set after configuration is done: 
#   OpenCV_FOUND
#   OpenCV_INCLUDE_DIRS
#   OpenCV_LIBRARIES
#   OpenCV_LINK_DIRECTORIES
#
#  Example:
#   FIND_PACKAGE(OpenCV)
#   INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
#   TARGET_LINK_LIBRARIES( yourproject ${OpenCV_LIBRARIES} )
#
# These variables are deprecated (and therefore not set!):
#  OPENCV_* uppercase replaced by case sensitive OpenCV_*
#  OPENCV_EXE_LINKER_FLAGS
#  OPENCV_INCLUDE_DIR : replaced by plural *_DIRS
# 
#
# History:
#
# 2004/05 Jan Woetzel, Friso, Daniel Grest 
# 2006/01 complete rewrite by Jan Woetzel
# 2006/09 2nd rewrite introducing ROOT_DIR and PATH_SUFFIXES 
#         to handle multiple installed versions gracefully by Jan Woetzel
#
# tested with:
# -OpenCV 0.97 (beta5a):  MSVS 7.1, gcc 3.3, gcc 4.1
# -OpenCV 0.99 (1.0rc1):  MSVS 7.1
#
# www.mip.informatik.uni-kiel.de/~jw
#
# 2009/06 Frederic Jean, 
#         Computer Vision and Systems Laboratory
#         Laval University, Quebec, QC, Canada
#	  http://vision.gel.ulaval.ca/~fjean
#
#         - Added support for new components in OpenCV 1.0
#         - Specifying components actually works now
#         - Added to possible root search the registry key used
#           by OpenCV 1.0 on Windows (no "Intel (R)" in key name)
#         - Added "include/opencv/cv.h" entry to find OpenCV root directory
#           (OpenCV 1.0 headers are now installed by default in 
#           "include/opencv", at least on Linux...)
#	  - Added output messages to know what is going on
#
#


#
# Required cv components with header and library if COMPONENTS unspecified
#
IF(NOT OpenCV_FIND_COMPONENTS)
  # Defaults
  SET(OpenCV_FIND_COMPONENTS CV CXCORE CVAUX HIGHGUI)
#  IF(WIN32)
#    LIST(APPEND OpenCV_FIND_COMPONENTS  CVCAM ) # WIN32 only actually
#  ENDIF(WIN32)  
ENDIF(NOT OpenCV_FIND_COMPONENTS)


#
# Typical root dirs of installations, exactly one of them is used
#
SET (OpenCV_POSSIBLE_ROOT_DIRS
  "${OpenCV_ROOT_DIR}"
  "$ENV{OpenCV_ROOT_DIR}"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Intel(R) Open Source Computer Vision Library_is1;Inno Setup: App Path]"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Open Source Computer Vision Library_is1;Inno Setup: App Path]"
  "$ENV{ProgramFiles}/OpenCV"
  /usr/local
  /usr
  )


#
# Select exactly ONE OpenCV base directory/tree 
# to avoid mixing different version headers and libs
#
FIND_PATH(OpenCV_ROOT_DIR 
  NAMES 
  cv/include/cv.h
  include/cv/cv.h
  include/opencv/cv.h  
  include/cv.h 
  PATHS ${OpenCV_POSSIBLE_ROOT_DIRS})


#
# Header include dir suffixes appended to OpenCV_ROOT_DIR
#
SET(OpenCV_INCDIR_SUFFIXES
  include
  include/cv
  include/opencv
  cv/include
  cxcore/include
  cvaux/include
  otherlibs/cvcam/include
  otherlibs/highgui
  otherlibs/highgui/include
  otherlibs/_graphics/include
  )


#
# Library linkdir suffixes appended to OpenCV_ROOT_DIR 
#
SET(OpenCV_LIBDIR_SUFFIXES
  lib
  OpenCV/lib
  otherlibs/_graphics/lib
  )


#
# Find incdir for each lib
#
FIND_PATH(OpenCV_CV_INCLUDE_DIR
  NAMES cv.h      
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
FIND_PATH(OpenCV_CXCORE_INCLUDE_DIR   
  NAMES cxcore.h
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
FIND_PATH(OpenCV_CVAUX_INCLUDE_DIR    
  NAMES cvaux.h
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
FIND_PATH(OpenCV_HIGHGUI_INCLUDE_DIR  
  NAMES highgui.h 
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
FIND_PATH(OpenCV_ML_INCLUDE_DIR    
  NAMES ml.h 
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )
FIND_PATH(OpenCV_CVCAM_INCLUDE_DIR    
  NAMES cvcam.h 
  PATHS ${OpenCV_ROOT_DIR} 
  PATH_SUFFIXES ${OpenCV_INCDIR_SUFFIXES} )


#
# Find absolute path to all libraries 
# some are optionally, some may not exist on Linux
#
FIND_LIBRARY(OpenCV_CV_LIBRARY   
  NAMES cv opencv libcv111 libcv200 libcv210
  PATHS ${OpenCV_ROOT_DIR}  
  PATH_SUFFIXES  ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_CVAUX_LIBRARY
  NAMES cvaux lbcvaux111 lbcvaux200 lbcvaux210
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_CVCAM_LIBRARY   
  NAMES cvcam
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} ) 
FIND_LIBRARY(OpenCV_CVHAARTRAINING_LIBRARY
  NAMES cvhaartraining
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} ) 
FIND_LIBRARY(OpenCV_CXCORE_LIBRARY  
  NAMES cxcore libcxcore111 libcxcore200 libcxcore210
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_CXTS_LIBRARY   
  NAMES cxts libcxts200
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_HIGHGUI_LIBRARY  
  NAMES highgui libhighgui111 libhighgui200 libhighgui210
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_ML_LIBRARY  
  NAMES ml libml200 libml210
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )
FIND_LIBRARY(OpenCV_TRS_LIBRARY  
  NAMES trs
  PATHS ${OpenCV_ROOT_DIR}  PATH_SUFFIXES ${OpenCV_LIBDIR_SUFFIXES} )

#
# Logic selecting required libs and headers
#
SET(OpenCV_FOUND ON)
FOREACH(NAME ${OpenCV_FIND_COMPONENTS} )
  # Only good if header and library both found   
  IF (OpenCV_${NAME}_INCLUDE_DIR AND OpenCV_${NAME}_LIBRARY)
    LIST(APPEND OpenCV_INCLUDE_DIRS ${OpenCV_${NAME}_INCLUDE_DIR} )
    LIST(APPEND OpenCV_LIBRARIES    ${OpenCV_${NAME}_LIBRARY} )
	MESSAGE(STATUS "Looking for OpenCV - component ${NAME} found" )
  ELSE (OpenCV_${NAME}_INCLUDE_DIR AND OpenCV_${NAME}_LIBRARY)
	MESSAGE(STATUS "Looking for OpenCV - component ${NAME} NOT found")
    SET(OpenCV_FOUND OFF)
  ENDIF (OpenCV_${NAME}_INCLUDE_DIR AND OpenCV_${NAME}_LIBRARY)
  
ENDFOREACH(NAME)


#
# Get the link directory for rpath to be used with LINK_DIRECTORIES: 
#
IF (OpenCV_CV_LIBRARY)
  GET_FILENAME_COMPONENT(OpenCV_LINK_DIRECTORIES ${OpenCV_CV_LIBRARY} PATH)
ENDIF (OpenCV_CV_LIBRARY)

MARK_AS_ADVANCED(
  OpenCV_ROOT_DIR
  OpenCV_INCLUDE_DIRS
  OpenCV_CV_INCLUDE_DIR
  OpenCV_CXCORE_INCLUDE_DIR
  OpenCV_CVAUX_INCLUDE_DIR
  OpenCV_ML_INCLUDE_DIR
  OpenCV_CVCAM_INCLUDE_DIR
  OpenCV_HIGHGUI_INCLUDE_DIR
  OpenCV_LIBRARIES
  OpenCV_CV_LIBRARY
  OpenCV_CXCORE_LIBRARY
  OpenCV_CVAUX_LIBRARY
  OpenCV_CVCAM_LIBRARY
  OpenCV_CVHAARTRAINING_LIBRARY
  OpenCV_CXTS_LIBRARY
  OpenCV_HIGHGUI_LIBRARY
  OpenCV_ML_LIBRARY
  OpenCV_TRS_LIBRARY
  )

#
# Display help message
#
IF(NOT OpenCV_FOUND)
  # Make FIND_PACKAGE friendly
  IF(NOT OpenCV_FIND_QUIETLY)
    IF(OpenCV_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR
		"OpenCV required but some headers or libs not found. Please specify it's location with OpenCV_ROOT_DIR env. variable.")
    ELSE(OpenCV_FIND_REQUIRED)
	  MESSAGE(STATUS "Looking for OpenCV - not found")
    ENDIF(OpenCV_FIND_REQUIRED)
  ENDIF(NOT OpenCV_FIND_QUIETLY)
ELSE(NOT OpenCV_FOUND)
  MESSAGE(STATUS "Looking for OpenCV - found in ${OpenCV_ROOT_DIR}")
ENDIF(NOT OpenCV_FOUND)
