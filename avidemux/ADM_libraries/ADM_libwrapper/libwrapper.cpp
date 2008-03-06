#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include "ADM_default.h"
#ifdef __MINGW32__
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "libwrapper.h"

ADM_LibWrapper::ADM_LibWrapper()
{
	initialised = false;
	hinstLib = NULL;
}

ADM_LibWrapper::~ADM_LibWrapper()
{
	if (hinstLib != NULL)
	{
		printf("Unloading library 0x%08x\n", hinstLib);

	#ifdef __MINGW32__
		FreeLibrary((HINSTANCE) hinstLib);
	#else
		dlclose(hinstLib);
	#endif
	}
}

bool ADM_LibWrapper::isAvailable()
{
	return initialised;
}

#ifdef __MINGW32__
char* ADM_LibWrapper::formatMessage(uint32_t msgCode)
{
	char* lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, msgCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);

	return lpMsgBuf;
}
#endif

bool ADM_LibWrapper::loadLibrary(const char* path)
{
#ifdef __MINGW32__
	hinstLib = LoadLibrary(path);

	if (hinstLib == NULL)
	{
		char* lpMsg = formatMessage(GetLastError());

		printf("Unable to load [%s]: %s\n", path, lpMsg);
		LocalFree(lpMsg);

		return false;
	}
	else
	{
		printf("Loaded library %s, handle = 0x%08x\n", path, hinstLib);

		return true;
	}
#else
	hinstLib = dlopen(path, RTLD_NOW);
	
	if (hinstLib == NULL)
	{
		printf("Unable to load [%s]: %s\n", path, dlerror());

		return false;
	}
	else
	{
		printf("Loaded library %s, handle = 0x%08x\n", path, hinstLib);

		return true;
	}
#endif
}

void* ADM_LibWrapper::getSymbol(const char* name)
{
#ifdef __MINGW32__
	void* procAddr = (void*)GetProcAddress((HINSTANCE) hinstLib, name);

	if (procAddr == NULL)
	{
		char* lpMsg = formatMessage(GetLastError());

		printf("Unable to find symbol [%s]: %s\n", name, lpMsg);
		LocalFree(lpMsg);
	}

	return procAddr;
#else
	void* procAddr = dlsym(hinstLib, name);

	if (procAddr == NULL)
	{
		printf("Unable to find symbol [%s]: %s\n", name, dlerror());
	}

	return procAddr;
#endif
}

bool ADM_LibWrapper::getSymbols(int symCount, ...)
{
#ifdef __MINGW32__
    va_list va;
    va_start(va, symCount);

    void** procFunction;
    char* funcName;
    int idxCount = 0;

    while (idxCount < symCount)
    {
        procFunction = va_arg(va, void**);
        funcName = va_arg(va, char*);

        if ((*procFunction = getSymbol(funcName)) == NULL)
        {
            procFunction = NULL;
            return false;
        }

        idxCount++;
    }

    va_end(va);
    return true;
#endif
}
