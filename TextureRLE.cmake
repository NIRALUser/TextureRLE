add_subdirectory(src)


OPTION(BUILD_TEST "Build testing?")

IF(BUILD_TEST)
	enable_testing()
	add_test(
		NAME ComponentSizeRLE 
		COMMAND ComponentSizeRLE --i /Users/prieto/Desktop/scanRescanOut/sub013.nii.gz --outputRLE temp.csv
		)
ENDIF(BUILD_TEST)