


SET(EXE_NAME test)
SET(${EXE_NAME}_LIBRARIES ${ITK_LIBRARIES})

add_executable(${EXE_NAME} ${EXE_NAME}.cxx)
target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})
