####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libADM_videoFilter.a

## INCLUDES were found outside kdevelop specific part

libADM_videoFilter_a_METASOURCES = AUTO
#INCLUDES = $(all_includes) 

libADM_videoFilter_a_SOURCES =  \
 ADM_vidForcedPP.cpp \
 ADM_vidMSharpen.cpp \
 ADM_vidDenoise.cpp \
 ADM_vidMPLD3D.cpp \
 ADM_vidMPLD3Dlow.cpp \
 ADM_lavpp_deint.cpp \
 ADM_vidKernelDeint.cpp \
 ADM_vidMcDeint.cpp \
 ADM_vidFade.cpp \
 ADM_vidResampleFPS.cpp \
 ADM_vidSoften.cpp \
 ADM_vidBSmear.cpp \
 ADM_vidAddBorder.cpp \
 ADM_vidFlux.cpp \
 ADM_vidMsmooth.cpp \
 ADM_vidFastConvolution.cpp 
			


####### kdevelop will overwrite this part!!! (end)############
INCLUDES = $(all_includes) -I../ADM_libraries -I../ADM_libraries/ADM_utilities -I../ADM_libraries/ADM_lavutil \
			 $(FREETYPE_CFLAGS) $(ALTIVEC_SWITCH) -I../ADM_userInterfaces/ADM_commonUI -I../ADM_inputs -I../ADM_video
