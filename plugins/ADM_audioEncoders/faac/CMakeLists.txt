	INCLUDE(ae_plugin)

	SET(ADM_ae_faac_SRCS audioencoder_faac.cpp)

	ADD_LIBRARY(ADM_ae_faac SHARED ${ADM_ae_faac_SRCS})

	TARGET_LINK_LIBRARIES(ADM_ae_faac ${FAAC_LIBRARY_DIR} ADM_core ADM_coreAudio ADM_coreUI)

	ADD_TARGET_CFLAGS(ADM_ae_faac "-I${FAAC_INCLUDE_DIR}")

	INIT_AUDIO_ENCODER(ADM_ae_faac)
	INSTALL_AUDIOENCODER(ADM_ae_faac)
