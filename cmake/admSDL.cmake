# - Locate the SDL library using sdl-config
# This module defines
#  SDL_LIBRARY, the library to link against
#  SDL_FOUND, if false, do not try to link to SDL
#  SDL_INCLUDE_DIR, where to find SDL.h
#
#MACRO(ADM_FIND_SDL)
#
#FIND_PROGRAM(SDL_EXECUTABLE
#  NAMES sdl-config
#  PATHS
#  $ENV{SDLDIR}/bin
#)
#if(SDL_EXECUTABLE)
#  MESSAGE(STATUS "sdl-config found ${SDL_EXECUTABLE}")
## Check we can run it
#    FIND_PROGRAM(SH_EXECUTABLE NAMES sh)
#
#    IF(SH_EXECUTABLE)
#      MESSAGE(STATUS "sh found ${SH_EXECUTABLE}")
#    
#      EXEC_PROGRAM(${SH_EXECUTABLE} ARGS "${SDL_EXECUTABLE} --libs"   OUTPUT_VARIABLE SDL_LDFLAGS )
#      EXEC_PROGRAM(${SH_EXECUTABLE} ARGS "${SDL_EXECUTABLE} --cflags" OUTPUT_VARIABLE SDL_CFLAGS )
#      SET(SDL_FOUND TRUE)
#    ELSEIF(WIN32)
#      SET(SDL_FOUND TRUE)
#      SET(SDL_LDFLAGS "-L/mingw/lib -lmingw32 -lSDLmain -lSDL -mwindows")
#      SET(SDL_CFLAGS "-I/mingw/include/SDL -D_GNU_SOURCE=1 -Dmain=SDL_main")
#    ENDIF(SH_EXECUTABLE)
#    
#    IF(SDL_FOUND)
#      MESSAGE(STATUS "cflags : ${SDL_CFLAGS}")
#      MESSAGE(STATUS "libs   : ${SDL_LDFLAGS}")
#    ENDIF(SDL_FOUND)
#else(SDL_EXECUTABLE)
#  MESSAGE(STATUS "sdl-config not found ")
#endif(SDL_EXECUTABLE)
#
#ENDMACRO(ADM_FIND_SDL)

MACRO(SDLify _source)
	if(SDL_FOUND)
        SET_SOURCE_FILES_PROPERTIES(${_source} PROPERTIES COMPILE_FLAGS "-I${SDL_INCLUDE_DIR}")
	endif(SDL_FOUND)
ENDMACRO(SDLify)
