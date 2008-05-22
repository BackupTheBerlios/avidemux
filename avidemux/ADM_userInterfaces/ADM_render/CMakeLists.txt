SET(ADM_LIB ADM_render)

SET(${ADM_LIB}_SRCS
GUI_render.cpp
GUI_sdlRender.cpp
GUI_xvRender.cpp
)

IF (APPLE)
	SET(${ADM_LIB}_SRCS ${${ADM_LIB}_SRCS} GUI_sdlRenderHelper.m)
ENDIF (APPLE)

ADD_ADM_SHARED_LIB_ALL_TARGETS(${ADM_LIB} ${${ADM_LIB}_SRCS})
ADM_TARGET_LINK_LIBARIES_ALL_TARGET( ${ADM_LIB} ADM_core ADM_coreUI ADM_coreImage)
SDLify(GUI_sdlRender.cpp)
