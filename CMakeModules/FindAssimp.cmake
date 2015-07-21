# - Try to find Assimp
# Once done, this will define
#
# Assimp_FOUND - system has Assimp
# Assimp_INCLUDE_DIR - the Assimp include directories
# Assimp_LIBRARIES - link these to use Assimp

FIND_PATH( Assimp_INCLUDE_DIR assimp/mesh.h
  /usr/include
  /usr/local/include
  /opt/local/include
  ${CMAKE_SOURCE_DIR}/3rdPartyLibraries/Assimp/include
)

FIND_LIBRARY( Assimp_LIBRARY
  NAMES assimp assimp32.lib
  /usr/lib64
  /usr/lib
  /usr/local/lib
  /opt/local/lib
  ${CMAKE_SOURCE_DIR}/3rdPartyLibraries/Assimp/lib32
)

IF(Assimp_INCLUDE_DIR AND Assimp_LIBRARY)
  SET( Assimp_FOUND TRUE )
  SET( Assimp_LIBRARIES ${Assimp_LIBRARY} )
ENDIF(Assimp_INCLUDE_DIR AND Assimp_LIBRARY)

SET(Assimp_FIND_QUIETLY FALSE)
IF(Assimp_FOUND)
   IF(NOT Assimp_FIND_QUIETLY)
      MESSAGE(STATUS "Found Assimp: ${Assimp_LIBRARY}")
   ENDIF(NOT Assimp_FIND_QUIETLY)
ELSE(Assimp_FOUND)
   IF(Assimp_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find libAssimp")
   ENDIF(Assimp_FIND_REQUIRED)
ENDIF(Assimp_FOUND)