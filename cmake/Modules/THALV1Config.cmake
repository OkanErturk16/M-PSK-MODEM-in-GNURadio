INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_THALV1 THALV1)

FIND_PATH(
    THALV1_INCLUDE_DIRS
    NAMES THALV1/api.h
    HINTS $ENV{THALV1_DIR}/include
        ${PC_THALV1_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    THALV1_LIBRARIES
    NAMES gnuradio-THALV1
    HINTS $ENV{THALV1_DIR}/lib
        ${PC_THALV1_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/THALV1Target.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(THALV1 DEFAULT_MSG THALV1_LIBRARIES THALV1_INCLUDE_DIRS)
MARK_AS_ADVANCED(THALV1_LIBRARIES THALV1_INCLUDE_DIRS)
