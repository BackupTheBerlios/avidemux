// Symbol mangling for asm
// is different on win32
// Shamelessly borrowed from lavcodec

#ifdef CYG_MANGLING // CYGWIN
	#define Mangle(x) "_" #x
	#define MANGLE(x) "_" #x
#else
	#define Mangle(x) #x
	#define MANGLE(x) #x
#endif
