# - Locate the SDL library using sdl-config
# This module defines
#  SDL_LIBRARY, the library to link against
#  SDL_FOUND, if false, do not try to link to SDL
#  SDL_INCLUDE_DIR, where to find SDL.h
#
MACRO(ADM_FIND_SDL)

PKGCONFIG(sdl SDL_INCLUDE SDL_LIB SDL_LDFLAGS SDL_CFLAGS)

IF(NOT SDL_LDFLAGS)
    MESSAGE(STATUS "Could not find SDL")
ELSE(NOT SDL_LDFLAGS)
    SET(SDL_FOUND TRUE)
    SET(SDL_CFLAGS ${SDL_CFLAGS})
    SET(SDL_LDFLAGS ${SDL_LDFLAGS})
    MESSAGE(STATUS "LDFLAGS <${SDL_LDFLAGS}>")
    MESSAGE(STATUS "CFLAGS <${SDL_CFLAGS}>")
    MESSAGE(STATUS OK)
    MESSAGE(STATUS "SDL Found")
ENDIF(NOT SDL_LDFLAGS)
ENDMACRO(ADM_FIND_SDL)

MACRO(SDLify _source)
if(SDL_FOUND)
        SET_SOURCE_FILES_PROPERTIES(${_source}
                                     PROPERTIES COMPILE_FLAGS ${SDL_CFLAGS}) 
endif(SDL_FOUND)
ENDMACRO(SDLify _source)
