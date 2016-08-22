#----------------------------------------------------------------------------
# USER! : SET THE NAME OF YOUR LIBRARY
# (Replace 'MyLib' by your own library name)

#############################
SET ( LIBRARY_NAME   itkT2RelaxationMap  )
#############################

#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# CREATES A USER OPTION IN CMAKE
OPTION ( BUILD_${LIBRARY_NAME}  "Build ${LIBRARY_NAME} library ?" ON)
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
IF ( BUILD_${LIBRARY_NAME} )
#----------------------------------------------------------------------------
  #----------------------------------------------------------------------------
  # BUILD LIBRARY
  #----------------------------------------------------------------------------

  #----------------------------------------------------------------------------
  FILE(GLOB ${LIBRARY_NAME}_HEADERS "*.h" "*.hxx" "*.txx")
  
  # OR MANUALLY LIST YOUR HEADERS WITH NEXT COMMAND
  #  SET ( ${LIBRARY_NAME}_HEADERS
  #
  #      )
  #----------------------------------------------------------------------------

  #----------------------------------------------------------------------------
  # LIBRARY SOURCES (TO BE COMPILED)
  # EITHER LIST ALL .cxx, *.cpp, *.cc IN CURRENT DIR USING NEXT LINE:

  FILE(GLOB ${LIBRARY_NAME}_SOURCES *.cxx *.cpp *.cc)

  

  #---------- Uncomment if you are using Qt in this library
  #FILE(GLOB ${LIBRARY_NAME}_UIS "*.ui")
  #QT4_WRAP_UI(${LIBRARY_NAME}_UIS_H ${${LIBRARY_NAME}_UIS})
  #QT4_WRAP_CPP(${LIBRARY_NAME}_SOURCES ${${LIBRARY_NAME}_HEADERS})
  #FILE(GLOB ${LIBRARY_NAME}_HEADERS "*.h" "*.txx" "${CMAKE_CURRENT_BINARY_DIR}/*.h")

  #----------------------------------------------------------------------------
  # LIBRARY DEPENDENCIES (LIBRARIES TO LINK WITH)
  #
  # USER! : Uncomment the Libraries you need
  #
  SET ( ${LIBRARY_NAME}_LINK_LIBRARIES
  #    ${VTK_LIBRARIES}
      ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}
  #    ${XERCES_LIBRARIES}

      )
  #----------------------------------------------------------------------------

  #----------------------------------------------------------------------------  
  CREA_ADD_LIBRARY(${LIBRARY_NAME})
  #CREA_ADD_CUSTOM_TARGET(${LIBRARY_NAME})

  #----------------------------------------------------------------------------

  #---------------------------------------------------------------------------
ENDIF ( BUILD_${LIBRARY_NAME} )
