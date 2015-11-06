
cmake_minimum_required(VERSION 3.3)

SET(FLAGS "--bind -s EXPORTED_FUNCTIONS=\"['_executeRLE', '_writeString', '_MountDirectory']\" -Wno-warn-absolute-paths --memory-init-file 0 -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1")

SET (CMAKE_CXX_FLAGS ${FLAGS} CACHE STRING "compile flags" FORCE)
SET (CMAKE_C_FLAGS ${FLAGS} CACHE STRING "compile flags" FORCE)

set(ITK_IO_MODULES_USED 
		ITKIONIFTI
		ITKIONRRD
	)
find_package(ITK COMPONENTS
  ITKCommon
  ITKIOImageBase
  ITKImageFunction
  ITKImageGrid
  ITKLabelMap
  ITKRegionGrowing
  ${ITK_IO_MODULES_USED}
)
IF(${ITK_FOUND})
	include(${ITK_USE_FILE})
ENDIF(${ITK_FOUND})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../lib/itkTextureFilter)
INCLUDE_DIRECTORIES(ENV{EMSCRIPTEN}/system/include/emscripten/)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib/itkJavascript)


SET(SLICER_EXECUTION_MODEL_DIR "" CACHE PATH "Slicerexecution model source dir")
include_directories(${SLICER_EXECUTION_MODEL_DIR}/tclap/include/)
include_directories(${SLICER_EXECUTION_MODEL_DIR}/ModuleDescriptionParser)

FIND_FILE(GENERATECLP_EXE NAMES GenerateCLP HINTS "${SLICER_EXECUTION_MODEL_DIR}/bin/GenerateCLP/bin/")

SET(EXE_NAME ComponentSizeRLE)
SET(${EXE_NAME}_LIBRARIES  ${ITK_LIBRARIES})
SET(${EXE_NAME}_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/../appli/${EXE_NAME}.cxx)


set(${EXE_NAME}_XML_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/../appli/${EXE_NAME}.xml)
SET(${EXE_NAME}_GENERATE_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}CLP.h)

SET(GENERATE ${GENERATECLP_EXE} --InputXML ${${EXE_NAME}_XML_INPUT} --OutputCxx ${${EXE_NAME}_GENERATE_OUTPUT})
add_custom_command(
      OUTPUT ${${EXE_NAME}_GENERATE_OUTPUT}
      COMMAND ${GENERATE}
      DEPENDS ${GENERATECLP_EXE} ${${EXE_NAME}_XML_INPUT}
      VERBATIM
      )
add_custom_target(GENERATECLP DEPENDS ${${EXE_NAME}_GENERATE_OUTPUT})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(EXE_NAME ComponentSizeRLE)
SET(${EXE_NAME}_LIBRARIES  ${ITK_LIBRARIES})
add_executable(${EXE_NAME}   ${${EXE_NAME}_SOURCE})
target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})
install(TARGETS ${${EXE_NAME}}
        RUNTIME
        DESTINATION bin)
ADD_DEPENDENCIES(${EXE_NAME} GENERATECLP)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ComponentSizeRLEExample.js ${CMAKE_CURRENT_BINARY_DIR}/ComponentSizeRLEExample.js
  COPYONLY)

