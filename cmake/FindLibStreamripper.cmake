# - Try to find libstreamripper
# Once done, this will define
#
#   LIBSTREAMRIPPER_FOUND - System has libstreamripper
#   LIBSTREAMRIPPER_INCLUDE_DIRS - The libstreamripper include directories
#   LIBSTREAMRIPPER_LIBRARIES - The libraries needed to use libstreamripper
#   LIBSTREAMRIPPER_DEFINITIONS - Compiler switches required for using libstreamripper

IF (LIBSTREAMRIPPER_LIBRARIES AND LIBSTREAMRIPPER_INCLUDE_DIRS )
    # in cache already
    SET(LIBSTREAMRIPPER_FOUND TRUE)
ELSE (LIBSTREAMRIPPER_LIBRARIES AND LIBSTREAMRIPPER_INCLUDE_DIRS )
    INCLUDE(FindPkgConfig)
    PKG_SEARCH_MODULE( LIBSTREAMRIPPER ${_pkgconfig_REQUIRED} libstreamripper )

    IF ( NOT LIBSTREAMRIPPER_FOUND AND NOT PKG_CONFIG_FOUND )
        FIND_PATH(
            _libstreamripper_include_DIR
            NAMES
            libstreamripper.h
            PATHS
            /usr/local/include
            ${CMAKE_LIBRARY_PATH}
            PATH_SUFFIXES
            srteamripper/include
            )
        FIND_LIBRARY(
            _libstreamripper_link_DIR
            NAMES
            libstreamripper
            PATHS
            /opt/gnome/lib
            /opt/local/lib
            /sw/lib
            /usr/lib
            /usr/local/lib
            )
        IF ( _libstreamripper_include_DIR AND _libstreamripper_link_DIR )
            SET ( _libstreamripper_FOUND TRUE )
        ENDIF ( _libstreamripper_include_DIR AND _libstreamripper_link_DIR )
        IF ( _libstreamripper_FOUND )
            SET ( LIBSTREAMRIPPER_INCLUDE_DIRS ${_libstreamripper_include_DIR} )
            SET ( LIBSTREAMRIPPER_LIBRARIES ${_libstreamripper_link_DIR} )
            SET ( LIBSTREAMRIPPER_FOUND TRUE )
        ELSE ( _libstreamripper_FOUND )
            SET ( LIBSTREAMRIPPER_FOUND FALSE )
        ENDIF ( _libstreamripper_FOUND )
    ENDIF ( NOT LIBSTREAMRIPPER_FOUND AND NOT PKG_CONFIG_FOUND )
    IF (LIBSTREAMRIPPER_FOUND)
        MESSAGE (STATUS "Found libstreamripper: ${LIBSTREAMRIPPER_LIBRARIES} ${LIBSTREAMRIPPER_INCLUDE_DIRS}")
    ELSE (LIBSTREAMRIPPER_FOUND)
        IF (LIBSTREAMRIPPER_FIND_REQUIRED)
            MESSAGE (SEND_ERROR "Could not find libstreamripper")
        ENDIF (LIBSTREAMRIPPER_FIND_REQUIRED)
    ENDIF (LIBSTREAMRIPPER_FOUND)
ENDIF (LIBSTREAMRIPPER_LIBRARIES AND LIBSTREAMRIPPER_INCLUDE_DIRS)
