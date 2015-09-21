
FILE(GLOB_RECURSE NAMES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*.sh)
FOREACH( NAME  ${NAMES})
  CONFIGURE_FILE(
      ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}
      ${PROJECT_BINARY_DIR}/appli/${NAME}
      COPYONLY )
ENDFOREACH(NAME)


SET(EXE_NAME ComputeTexture)
SET(${EXE_NAME}_LIBRARIES  ${ITK_LIBRARIES})

add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})
install(TARGETS ${EXE_NAME}
        RUNTIME
        DESTINATION bin)




SET(EXE_NAME TextureBioMarkerTool)
SET(${EXE_NAME}_LIBRARIES  ${ITK_LIBRARIES})

add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
set_target_properties(${EXE_NAME} PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})
install(TARGETS ${EXE_NAME}
        RUNTIME
        DESTINATION bin)


SET(EXE_NAME polyfit)
SET(${EXE_NAME}_LIBRARIES  ${ITK_LIBRARIES})

add_executable(${EXE_NAME}   ${EXE_NAME}.c)
set_target_properties(${EXE_NAME} PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})
install(TARGETS ${EXE_NAME}
        RUNTIME
        DESTINATION bin)