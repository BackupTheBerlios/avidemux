####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libADM_audiofilter.a

#INCLUDES = $(all_includes)

libADM_audiofilter_a_METASOURCES = AUTO

libADM_audiofilter_a_SOURCES = audiodeng_buildfilters.cpp audioeng_ffmp2.cpp audioeng_tshift.cpp audioeng_rawshift.cpp audioeng_null.cpp audioeng_normalize.cpp audioeng_mono.cpp audioeng_lame.cpp audioeng_buff.cpp audioeng_6dbpress.cpp audioeng_44sample.cpp audioprocess.cpp audioeng_toolame.cpp audioeng_toolame.h audioeng_piper.cpp audioeng_piper.h audioeng_exlame.cpp audioeng_app.cpp audioeng_lebe.cpp audioeng_soxresample.cpp audioeng_sox.cpp ADM_libresample.cpp audioeng_libtoolame.cpp audioeng_film2pal.cpp


EXTRA_DIST = audioeng_44sample.cpp audioeng_6dbpress.cpp audioeng_buff.cpp audioeng_lame.cpp audioeng_mono.cpp audioeng_normalize.cpp audioeng_null.cpp audioeng_rawshift.cpp audioeng_tshift.cpp audioprocess.hxx audioeng_ffmp2.cpp audioeng_ffmp2.h audiodeng_buildfilters.cpp audioeng_buildfilters.h audioprocess.cpp resampl.h

####### kdevelop will overwrite this part!!! (end)############

INCLUDES= $(all_includes) $(GTK_CFLAGS) -DTOOLAME='"'$(TOOLAME)'"' -DEXLAME='"'$(EXLAME)'"'
noinst_HEADERS = audioeng_sox.h audioeng_libtoolame.h audioeng_film2pal.h
