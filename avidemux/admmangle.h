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
/* Regiter renaming */
#ifdef HAVE_X86_64
#define REG_ax  "%%rax" 
#define REG_bx  "%%rbx" 
#define REG_cx  "%%rcx" 
#define REG_dx  "%%rdx" 
#define REG_si  "%%rsi" 
#define REG_di  "%%rdi" 
#else
#define REG_ax  "%%eax" 
#define REG_bx  "%%ebx" 
#define REG_cx  "%%ecx" 
#define REG_dx  "%%edx" 
#define REG_si  "%%esi" 
#define REG_di  "%%edi" 
#endif	

