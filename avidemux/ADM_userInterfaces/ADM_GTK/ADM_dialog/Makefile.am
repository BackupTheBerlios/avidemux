
noinst_LIBRARIES = libADM_dialog.a 

INCLUDES = $(all_includes) $(GTK_CFLAGS) $(XML_CFLAGS)  $(FREETYPE_CFLAGS) -DADM_SUBVERSION=@ADM_SUBVERSION@ \
		-I../../../ADM_libraries/ADM_utilities \
		-I../../ADM_commonUI -I../../.. -I../../../ADM_libraries/ADM_lavutil \
		-I../../../ADM_inputs -I../../../ADM_outputs

libADM_dialog_a_METASOURCES = AUTO

libADM_dialog_a_SOURCES =  DIA_vcodec.cpp DIA_acodec.cpp \
	DIA_properties.cpp  DIA_about.cpp DIA_recent.cpp \
	DIA_resize.cpp DIA_working.cpp \
	DIA_working.h DIA_busy.cpp DIA_busy.h DIA_exLame.cpp DIA_pipe.cpp  \
	DIA_xvid.cpp DIA_crop.cpp  DIA_encoding.cpp   \
	DIA_xvid4.cpp  \
	DIA_pause.cpp \
	DIA_calculator.cpp DIA_equalizer.cpp \
	DIA_vobsub.cpp DIA_x264.cpp DIA_ocr.cpp  DIA_hue.cpp DIA_eq2.cpp \
	DIA_asharp.cpp  DIA_idx_pg.cpp  \
	DIA_cnr2.cpp DIA_jobs.cpp DIA_jobs_save.cpp  \
	DIA_mpdelogo.cpp \
        DIA_chromaShift.cpp \
        \
        DIA_preview.cpp \
        DIA_contrast.cpp \
        DIA_srt.cpp \
	DIA_coloryuv.cpp \
	DIA_quota.cpp \
	DIA_partial.cpp  \
	DIA_flyDialog.cpp




EXTRA_DIST =  \
DIA_mcdeint.cpp     DIA_resize.cpp \
DIA_cnr2.cpp          \
DIA_color.cpp         DIA_eq2.cpp          \
DIA_coloryuv.cpp      DIA_equalizer.cpp    DIA_mpdelogo.cpp     \
DIA_contrast.cpp      DIA_exLame.cpp       DIA_srt.cpp \
DIA_about.cpp         \
DIA_acodec.cpp       DIA_crop.cpp          \
DIA_hue.cpp          DIA_ocr.cpp         DIA_vcodec.cpp \
DIA_asharp.cpp       DIA_idx_pg.cpp       DIA_partial.cpp     DIA_vobsub.cpp \
DIA_idx_pg.h         DIA_pause.cpp       \
DIA_pipe.cpp        DIA_working.cpp \
DIA_defaultAudio.cpp  DIA_jobs.cpp        DIA_working.h \
DIA_jobs_save.cpp     DIA_x264.cpp \
DIA_preview.cpp     \
DIA_busy.cpp         DIA_properties.cpp  DIA_xvid.cpp \
DIA_busy.h            DIA_quota.cpp       DIA_xvid4.cpp \
DIA_calculator.cpp   DIA_encoding.cpp      DIA_recent.cpp \
DIA_chromaShift.cpp  DIA_encoding.h        DIA_lavpp_deint.cpp   


