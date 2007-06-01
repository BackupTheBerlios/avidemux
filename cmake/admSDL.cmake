# - Locate the SDL library using sdl-config
# This module defines
#  SDL_LIBRARY, the library to link against
#  SDL_FOUND, if false, do not try to link to SDL
#  SDL_INCLUDE_DIR, where to find SDL.h
#
MACRO(ADM_FIND_SDL)

FIND_PROGRAM(SDL_EXECUTABLE
  NAMES sdl-config
  PATHS
  $ENV{SDLDIR}/bin
)
if(SDL_EXECUTABLE)
  MESSAGE(STATUS "sdl-config found ${SDL_EXECUTABLE}")
# Check we can run it
    SET(_return_VALUE TRUE)
    IF(_return_VALUE)
      EXEC_PROGRAM(${SDL_EXECUTABLE} ARGS "--libs"   OUTPUT_VARIABLE SDL_LDFLAGS )
      EXEC_PROGRAM(${SDL_EXECUTABLE} ARGS "--cflags" OUTPUT_VARIABLE SDL_CFLAGS )
      SET(SDL_FOUND TRUE)
      MESSAGE(STATUS "cflags : ${SDL_CFLAGS}")
      MESSAGE(STATUS "libs   : ${SDL_LDFLAGS}")
  ENDIF(_return_VALUE)
else(SDL_EXECUTABLE)
  MESSAGE(STATUS "sdl-config not found ")
endif(SDL_EXECUTABLE)
ENDMACRO(ADM_FIND_SDL)