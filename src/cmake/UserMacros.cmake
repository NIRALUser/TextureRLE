
#========================================================================
# COPIES A DIRECTORY EXCLUDING CVS ENTRIES
MACRO(COPYDIR SOURCE DEST)
  FILE(GLOB_RECURSE NAMES RELATIVE ${SOURCE} ${SOURCE}/*)
  FOREACH( NAME  ${NAMES})
    STRING(SUBSTRING ${NAME} 0 3 subStrTmp )
    IF( NOT subStrTmp STREQUAL "../" )
      STRING(REGEX MATCH "CVS" CVSSUBS_FOUND ${NAME} )
      IF( NOT CVSSUBS_FOUND STREQUAL "CVS" )
	CONFIGURE_FILE(
	  ${SOURCE}/${NAME} 
	  ${DEST}/${NAME}
	  COPYONLY )
      ENDIF( NOT CVSSUBS_FOUND STREQUAL "CVS" )
    ENDIF( NOT subStrTmp STREQUAL "../" )
  ENDFOREACH(NAME)
ENDMACRO(COPYDIR)
#========================================================================

MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

# Creates and installs a library
# Uses CREA_DYNAMIC_LIBRARY_EXPORT and CREA_INSTALL_LIBRARY
# hence the variables to be set for these are needed 
# Additionnaly one needs to set :
# ${LIBRARY_NAME}_HEADERS : the list of headers to install
# ${LIBRARY_NAME}_SOURCES : the list of files to compile to build the lib
# ${LIBRARY_NAME}_LINK_LIBRARIES : the list of libs to link with
MACRO(CREA_ADD_LIBRARY LIBRARY_NAME)


  MESSAGE(STATUS "===============================================")
  MESSAGE(STATUS "Configuring ${LIBRARY_NAME}")
  MESSAGE(STATUS "   Headers  : ${${LIBRARY_NAME}_HEADERS}")
  MESSAGE(STATUS "   Sources  : ${${LIBRARY_NAME}_SOURCES}")
  MESSAGE(STATUS "   Link libs: ${${LIBRARY_NAME}_LINK_LIBRARIES}")
  MESSAGE(STATUS "===============================================")  
  

  # MANAGE SHARED LIB
  #DYNAMIC_LIBRARY_EXPORT(${LIBRARY_NAME})
  # CREATE THE TARGET
  ADD_LIBRARY(${LIBRARY_NAME} ${${LIBRARY_NAME}_SHARED} ${${LIBRARY_NAME}_SOURCES})

  # LINK
  TARGET_LINK_LIBRARIES(${LIBRARY_NAME} ${${LIBRARY_NAME}_LINK_LIBRARIES})

  # INSTALLS LIBRARY
  INSTALL(
    FILES ${${LIBRARY_NAME}_HEADERS}
    DESTINATION include/${LIBRARY_NAME}}
    )  

  INSTALL(
    TARGETS ${LIBRARY_NAME}
    DESTINATION lib )


ENDMACRO(CREA_ADD_LIBRARY)

# Creates and installs a library
# Uses CREA_DYNAMIC_LIBRARY_EXPORT and CREA_INSTALL_LIBRARY
# hence the variables to be set for these are needed 
# Additionnaly one needs to set :
# ${LIBRARY_NAME}_HEADERS : the list of headers to install
# ${LIBRARY_NAME}_SOURCES : the list of files to compile to build the lib
# ${LIBRARY_NAME}_LINK_LIBRARIES : the list of libs to link with
MACRO(CREA_ADD_CUSTOM_TARGET TARGET_NAME)


  MESSAGE(STATUS "===============================================")
  MESSAGE(STATUS "Configuring ${TARGET_NAME}")
  MESSAGE(STATUS "Headers  : ${${TARGET_NAME}_HEADERS}")
  MESSAGE(STATUS "===============================================")  

  SET(${TARGET_NAME}_SOURCES_ALL ${${TARGET_NAME}_SOURCES} ${${TARGET_NAME}_HEADERS})
  # CREATE THE TARGET
  add_custom_target(${LIBRARY_NAME} ${${TARGET_NAME}_SOURCES_ALL})

  # INSTALLS TARGET
  INSTALL(
    FILES ${${TARGET_NAME}_HEADERS}
    DESTINATION include/${LIBRARY_NAME}}
    )  

ENDMACRO(CREA_ADD_CUSTOM_TARGET)


# Manages the shared library creation/use 
# * Creates an option ${LIBRARY_NAME}_BUILD_SHARED 
# * Generates the file ${LIBRARY_NAME}_EXPORT.h which 
#   defines the symbols ${LIBRARY_NAME}_EXPORT and ${LIBRARY_NAME}_CDECL
#   to be used in exported classes/functions declarations
MACRO(DYNAMIC_LIBRARY_EXPORT LIBRARY_NAME)

  SET(${LIBRARY_NAME}_SHARED STATIC)

  IF (${BUILD_SHARED_LIBS})
    SET(${LIBRARY_NAME}_SHARED SHARED)
    CREA_DEFINE(${LIBRARY_NAME}_BUILD_SHARED)
    CREA_DEFINE(${LIBRARY_NAME}_EXPORT_SYMBOLS)
  ENDIF(${BUILD_SHARED_LIBS})

  # ADDS CURRENT BINARY DIR TO INCLUDE DIRS
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

  # CONFIGURES ${LIBRARY_NAME}_EXPORT.h
  CONFIGURE_FILE(
    ${USER_CMAKE_MACROS}/LIBRARY_NAME_EXPORT.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}_EXPORT.h
    @ONLY IMMEDIATE
    )
  # ADDS IT TO THE LIST OF HEADERS
  SET(${LIBRARY_NAME}_HEADERS
    ${${LIBRARY_NAME}_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}_EXPORT.h
    )

ENDMACRO(DYNAMIC_LIBRARY_EXPORT LIBRARY_NAME)


MACRO(ADVANCED_INSTALL_LIBRARY_FOR_CMAKE LIBRARY_NAME)

  #---------------------------------------------------------------------------
  # Sets the common values to build tree and install tree configs
  SET(CILC_LIB_REQUIRED_C_FLAGS    ${${LIBRARY_NAME}_REQUIRED_C_FLAGS})
  SET(CILC_LIB_REQUIRED_CXX_FLAGS  ${${LIBRARY_NAME}_REQUIRED_CXX_FLAGS})
  SET(CILC_LIB_REQUIRED_LINK_FLAGS ${${LIBRARY_NAME}_REQUIRED_LINK_FLAGS})
  SET(CILC_LIB_MAJOR_VERSION       ${${LIBRARY_NAME}_MAJOR_VERSION})
  SET(CILC_LIB_MINOR_VERSION       ${${LIBRARY_NAME}_MINOR_VERSION})
  SET(CILC_LIB_BUILD_VERSION       ${${LIBRARY_NAME}_BUILD_VERSION})
  SET(CILC_LIB_VERSION             
    ${CILC_LIB_MAJOR_VERSION}.${CILC_LIB_MINOR_VERSION}.${CILC_LIB_BUILD_VERSION})
  SET(CILC_LIB_LIBRARIES ${${LIBRARY_NAME}_LIBRARIES})
  IF (${LIBRARY_NAME}_HAS_ADDITIONAL_CONFIG_FILE)
    SET(CILC_LIB_HAS_ADDITIONAL_CONFIG_FILE TRUE)
  ELSE (${LIBRARY_NAME}_HAS_ADDITIONAL_CONFIG_FILE)
    SET(CILC_LIB_HAS_ADDITIONAL_CONFIG_FILE FALSE)
  ENDIF (${LIBRARY_NAME}_HAS_ADDITIONAL_CONFIG_FILE)
  #---------------------------------------------------------------------------


  #---------------------------------------------------------------------------
  #-- BUILD TREE configuration 
  SET(CILC_BUILD_TREE_CONFIGURATION TRUE)
  # UseLIBRARY_NAME.cmake 
  CONFIGURE_FILE(
    ${USER_CMAKE_MACROS}/InstallLibraryForCMake_UseLibrary.cmake.in
    ${PROJECT_BINARY_DIR}/Use${LIBRARY_NAME}.cmake
    @ONLY IMMEDIATE
    )
  # LIBRARY_NAMEConfig.cmake 
  SET(CILC_LIB_RELATIVE_INCLUDE_PATHS 
    ${${LIBRARY_NAME}_BUILD_TREE_RELATIVE_INCLUDE_PATHS})
  SET(CILC_LIB_RELATIVE_LIBRARY_PATHS 
    ${${LIBRARY_NAME}_BUILD_TREE_RELATIVE_LIBRARY_PATHS})
  CONFIGURE_FILE(
    ${USER_CMAKE_MACROS}/InstallLibraryForCMake_LibraryConfig.cmake.in
    ${PROJECT_BINARY_DIR}/${LIBRARY_NAME}Config.cmake
    @ONLY IMMEDIATE
    )
  # LIBRARY_NAMEBuildSettings.cmake :
  # Save the compiler settings so another project can import them.
  # FCY : DEPREDCATED FUNCTION FOR CMAKE 2.8
  IF( "${CMAKE_MINIMUM_REQUIRED_VERSION}" VERSION_LESS 2.7)
  INCLUDE(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
  CMAKE_EXPORT_BUILD_SETTINGS(${PROJECT_BINARY_DIR}/${LIBRARY_NAME}BuildSettings.cmake)
  ENDIF()
  # Additional Config and Use files
  IF(CILC_LIB_HAS_ADDITIONAL_CONFIG_FILE)
    CONFIGURE_FILE(
      ${${LIBRARY_NAME}_ADDITIONAL_CONFIG_FILE}
      ${PROJECT_BINARY_DIR}/Additional${LIBRARY_NAME}Config.cmake
      @ONLY IMMEDIATE
      )
    CONFIGURE_FILE(
      ${${LIBRARY_NAME}_ADDITIONAL_USE_FILE}
      ${PROJECT_BINARY_DIR}/AdditionalUse${LIBRARY_NAME}.cmake
      @ONLY IMMEDIATE
      )
  ENDIF(CILC_LIB_HAS_ADDITIONAL_CONFIG_FILE)
  #---------------------------------------------------------------------------
  

  #---------------------------------------------------------------------------
  #-- INSTALL TREE configuration 


  SET(CILC_BUILD_TREE_CONFIGURATION FALSE)
  # Create work directory to put the configured files because 
  # if the user invoked the macro at the root of the build tree the files 
  # would overwrite those configured for the build tree
  SET(CILC_WORK_DIR ${CMAKE_CURRENT_BINARY_DIR}/InstallLibraryForCMake_tmp)
  IF(NOT IS_DIRECTORY ${CILC_WORK_DIR})
    FILE(MAKE_DIRECTORY ${CILC_WORK_DIR})
  ENDIF(NOT IS_DIRECTORY ${CILC_WORK_DIR})
  
  #----------------------------------------------------------------------------
  SET(INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/lib/${LIBRARY_NAME}/cmake)
  

  message(STATUS "Library Install Path ${INSTALL_PATH}")

  # UseLIBRARY_NAME.cmake 
  CONFIGURE_FILE(
    ${USER_CMAKE_MACROS}/InstallLibraryForCMake_UseLibrary.cmake.in
    ${CILC_WORK_DIR}/Use${LIBRARY_NAME}.cmake
    @ONLY IMMEDIATE
    )
  INSTALL(
    FILES 
    ${CILC_WORK_DIR}/Use${LIBRARY_NAME}.cmake
    DESTINATION ${INSTALL_PATH}
    )
  # LIBRARY_NAMEConfig.cmake 
  SET(CILC_LIB_RELATIVE_INCLUDE_PATHS 
    ${${LIBRARY_NAME}_INSTALL_TREE_RELATIVE_INCLUDE_PATHS})
  SET(CILC_LIB_RELATIVE_LIBRARY_PATHS 
    ${${LIBRARY_NAME}_INSTALL_TREE_RELATIVE_LIBRARY_PATHS})
  CONFIGURE_FILE(
    ${USER_CMAKE_MACROS}/InstallLibraryForCMake_LibraryConfig.cmake.in
    ${CILC_WORK_DIR}/${LIBRARY_NAME}Config.cmake
    @ONLY IMMEDIATE
    )
  INSTALL(
    FILES 
    ${CILC_WORK_DIR}/${LIBRARY_NAME}Config.cmake
    DESTINATION ${INSTALL_PATH}
    )
  
  # Additional Config and Use files
  IF(CILC_LIB_HAS_ADDITIONAL_CONFIG_FILE)
    CONFIGURE_FILE(
      ${${LIBRARY_NAME}_ADDITIONAL_CONFIG_FILE}
      ${CILC_WORK_DIR}/Additional${LIBRARY_NAME}Config.cmake
      @ONLY IMMEDIATE
      )
    INSTALL(
      FILES 
      ${CILC_WORK_DIR}/Additional${LIBRARY_NAME}Config.cmake
      DESTINATION ${INSTALL_PATH}
      )
    CONFIGURE_FILE(
      ${${LIBRARY_NAME}_ADDITIONAL_USE_FILE}
      ${CILC_WORK_DIR}/AdditionalUse${LIBRARY_NAME}.cmake
      @ONLY IMMEDIATE
      )
    INSTALL(
      FILES 
      ${CILC_WORK_DIR}/AdditionalUse${LIBRARY_NAME}.cmake
      DESTINATION  ${INSTALL_PATH}
      )
  ENDIF(CILC_LIB_HAS_ADDITIONAL_CONFIG_FILE)
  # Specific to install tree
  # FindLIBRARY_NAME.cmake in CMake/Modules dir
  CONFIGURE_FILE(
    ${USER_CMAKE_MACROS}/InstallLibraryForCMake_FindLibrary.cmake.in
    ${CILC_WORK_DIR}/Find${LIBRARY_NAME}.cmake
    @ONLY IMMEDIATE
    )
#EED 02 mars 2011
#  INSTALL( 
#    FILES ${CILC_WORK_DIR}/Find${LIBRARY_NAME}.cmake
#    DESTINATION ${CMAKE_ROOT}/Modules 
#    )

# JPR 04 Mars 2011
  INSTALL( 
    FILES ${CILC_WORK_DIR}/Find${LIBRARY_NAME}.cmake
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/cmake/Modules
    )
    
 
  #---------------------------------------------------------------------------

ENDMACRO(ADVANCED_INSTALL_LIBRARY_FOR_CMAKE)
#-----------------------------------------------------------------------------


#-----------------------------------------------------------------------------
MACRO(INSTALL_LIBRARY_FOR_CMAKE LIBRARY_NAME1)
  
  # Sets the settings to default values
  IF(NOT ${LIBRARY_NAME1}_INSTALL_FOLDER)
    SET(${LIBRARY_NAME1}_INSTALL_FOLDER ${LIBRARY_NAME1})
  ENDIF(NOT ${LIBRARY_NAME1}_INSTALL_FOLDER)

  SET(${LIBRARY_NAME1}_LIBRARIES ${LIBRARY_NAME1})
  
  FILE(RELATIVE_PATH 
    ${LIBRARY_NAME1}_BUILD_TREE_RELATIVE_INCLUDE_PATHS 
    ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    )
 
  IF(UNIX)
    SET(${LIBRARY_NAME1}_BUILD_TREE_RELATIVE_LIBRARY_PATHS 
      ${CILFC_EXECUTABLE_OUTPUT_REL_PATH})
    SET(${LIBRARY_NAME1}_INSTALL_TREE_RELATIVE_LIBRARY_PATHS ${CMAKE_CREA_LIB_PATH} )
  ELSE(UNIX)
    SET(${LIBRARY_NAME1}_BUILD_TREE_RELATIVE_LIBRARY_PATHS 
      ${CILFC_EXECUTABLE_OUTPUT_REL_PATH} )
# LG 15/01/09 : Bug
#      ${CILFC_EXECUTABLE_OUTPUT_REL_PATH}/Debug
#     ${CILFC_EXECUTABLE_OUTPUT_REL_PATH}/Release)
    SET(${LIBRARY_NAME1}_INSTALL_TREE_RELATIVE_LIBRARY_PATHS bin)
  ENDIF(UNIX)

  IF(NOT ${LIBRARY_NAME1}_INSTALL_TREE_RELATIVE_INCLUDE_PATHS)
    SET(${LIBRARY_NAME1}_INSTALL_TREE_RELATIVE_INCLUDE_PATHS include/${LIBRARY_NAME1})
  ENDIF(NOT ${LIBRARY_NAME1}_INSTALL_TREE_RELATIVE_INCLUDE_PATHS)
  
  # Invoke the advanced macro
  ADVANCED_INSTALL_LIBRARY_FOR_CMAKE(${LIBRARY_NAME1})


ENDMACRO(INSTALL_LIBRARY_FOR_CMAKE)

#-----------------------------------------------------------------------------
# MACRO TO ADD A COMPILER DEFINITION WHICH ADDS ALSO THE DEF 
# INTO DOXYGEN PREDEFINED VAR 
# (OTHERWISE DOXYGEN WOULD NOT PARSE THE FILES BLOCKED)
MACRO(CREA_DEFINE DEF)
  MESSAGE(STATUS "* Added definition -D${DEF}")
  ADD_DEFINITIONS( -D${DEF} )
  SET(CREA_DEFINITIONS
    ${CREA_DEFINITIONS};${DEF}
    CACHE 
    STRING 
    ""
    FORCE)
  MARK_AS_ADVANCED(CREA_DEFINITIONS)
ENDMACRO(CREA_DEFINE)