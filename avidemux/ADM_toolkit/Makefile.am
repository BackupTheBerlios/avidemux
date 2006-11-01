###### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libADM_toolkit.a

## INCLUDES were found outside kdevelop specific part

libADM_toolkit_a_METASOURCES = AUTO

libADM_toolkit_a_SOURCES =  \
	ADM_audioQueue.cpp   automation.cpp    \
	ADM_intfloat.cpp     \
	ADM_packetQueue.cpp     

EXTRA_DIST = ADM_audioQueue.h  ADM_packetQueue.h         \
		ADM_intfloat.h    bitmap.h           \
		filesel.h       


####### kdevelop will overwrite this part!!! (end)############
INCLUDES = $(all_includes) -I../ADM_libraries -I../ADM_libraries/ADM_utilities -I../ADM_libraries/ADM_lavutil \
		 $(SMONKEY_CFLAGS)
noinst_HEADERS =  
