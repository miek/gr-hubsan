INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_HUBSAN hubsan)

FIND_PATH(
    HUBSAN_INCLUDE_DIRS
    NAMES hubsan/api.h
    HINTS $ENV{HUBSAN_DIR}/include
        ${PC_HUBSAN_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    HUBSAN_LIBRARIES
    NAMES gnuradio-hubsan
    HINTS $ENV{HUBSAN_DIR}/lib
        ${PC_HUBSAN_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(HUBSAN DEFAULT_MSG HUBSAN_LIBRARIES HUBSAN_INCLUDE_DIRS)
MARK_AS_ADVANCED(HUBSAN_LIBRARIES HUBSAN_INCLUDE_DIRS)

