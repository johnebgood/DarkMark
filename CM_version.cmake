# DarkMark (C) 2019-2020 Stephane Charette <stephanecharette@gmail.com>
# $Id$


EXECUTE_PROCESS (
	COMMAND svnversion
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	OUTPUT_VARIABLE DM_VER_SVN
	OUTPUT_STRIP_TRAILING_WHITESPACE )

IF ( NOT DM_VER_SVN MATCHES "[0-9]+.*" )
	# Maybe building from tarball without having done a svn checkout?  Use a fake version number
	SET ( DM_VER_SVN 1 )
	MESSAGE ( "exact version number is unavailable (building from source tarball?)" )
ENDIF ()

SET ( DM_VER_MAJOR 1 )
SET ( DM_VER_MINOR 0 )
SET ( DM_VER_PATCH 0-${DM_VER_SVN} )
SET ( DM_VERSION ${DM_VER_MAJOR}.${DM_VER_MINOR}.${DM_VER_PATCH} )
MESSAGE ( "Building ver: ${DM_VERSION}" )

ADD_DEFINITIONS ( -DDARKMARK_VERSION="${DM_VERSION}" )
