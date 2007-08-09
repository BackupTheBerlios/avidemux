#include "config.h"

#ifdef ADM_WIN32
#define WIN32_CLASH
#include <stdio.h>
#include <stdlib.h>

#include "default.h" 
#include "ADM_misc.h"
#include "windows.h"
#include "io.h"
#include "winsock2.h"
#include "ADM_assert.h" 

uint8_t win32_netInit(void);

void ADM_usleep(unsigned long us)
{
	Sleep(us/1000);
}

uint8_t win32_netInit(void)
{
WSADATA wsaData;
	int iResult;
		printf("Initializing WinSock\n");
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != NO_ERROR)
		{
			printf("Error at WSAStartup()\n");
			return 0;
		}	
		printf("WinSock ok\n");
		return 1;
}

#ifndef HAVE_GETTIMEOFDAY
extern "C"
{
void gettimeofday(struct timeval *p, void *tz);
}

void gettimeofday(struct timeval *p, void *tz)
{
    unsigned long int sec;
    SYSTEMTIME  tme;

    GetSystemTime(&tme);

    sec=tme.wSecond;
    sec+=60*tme.wMinute;
    sec+=60*60*tme.wHour;
    
	p->tv_sec=sec;
	p->tv_usec=tme.wMilliseconds*1000;

	return;
}
#endif

int getpriority(int which, int who)
{
	unsigned int priorityClass;

	ADM_assert(which == PRIO_PROCESS);
	ADM_assert(who == 0);

	priorityClass = GetPriorityClass(GetCurrentProcess());

	switch (priorityClass)
	{
		case HIGH_PRIORITY_CLASS:
			return -18;
			break;
		case ABOVE_NORMAL_PRIORITY_CLASS:
			return -10;
			break;
		case NORMAL_PRIORITY_CLASS:
			return 0;
			break;
		case BELOW_NORMAL_PRIORITY_CLASS:
			return 10;
			break;
		case IDLE_PRIORITY_CLASS:
			return 18;
			break;
		default:
			ADM_assert(0);
	}
}

int setpriority(int which, int who, int value)
{
	unsigned int priorityClass;

	ADM_assert(which == PRIO_PROCESS);
	ADM_assert(who == 0);
	ADM_assert(value >= PRIO_MIN && value <= PRIO_MAX);

	if (value >= -20 && value <= -16)
	{
		priorityClass = HIGH_PRIORITY_CLASS;
	}
	else if (value >= -15 && value <= -6)
	{
		priorityClass = ABOVE_NORMAL_PRIORITY_CLASS;
	}
	else if (value >= -5 && value <= 4)
	{
		priorityClass = NORMAL_PRIORITY_CLASS;
	}
	else if (value >= 6 && value <= 15)
	{
		priorityClass = BELOW_NORMAL_PRIORITY_CLASS;
	}
	else if (value >= 16 && value <= 20)
	{
		priorityClass = IDLE_PRIORITY_CLASS;
	}

	if (!SetPriorityClass(GetCurrentProcess(), priorityClass))
	{
		return -1;
	}

	return 0;
}

int shutdown_win32(void)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	// Get a token for this process. 
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return -1;
	}

	// Get the LUID for the shutdown privilege.
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;  // one privilege to set
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process.
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	if (GetLastError() != ERROR_SUCCESS)
	{
		return -1;
	}

	// Shut down the system and force all applications to close.
	if (!ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, SHTDN_REASON_FLAG_PLANNED))
	{
		return -1;
	}

	return 0;
}

#ifndef PRODUCT_BUSINESS
#define PRODUCT_BUSINESS 0x00000006
#endif

#ifndef PRODUCT_BUSINESS_N
#define PRODUCT_BUSINESS_N 0x00000010
#endif

#ifndef PRODUCT_HOME_BASIC
#define PRODUCT_HOME_BASIC 0x00000002
#endif

#ifndef PRODUCT_HOME_BASIC_N
#define PRODUCT_HOME_BASIC_N 0x00000005
#endif

#ifndef PRODUCT_HOME_PREMIUM
#define PRODUCT_HOME_PREMIUM 0x00000003
#endif 

#ifndef PRODUCT_STARTER
#define PRODUCT_STARTER 0x0000000B
#endif

#ifndef PRODUCT_ENTERPRISE
#define PRODUCT_ENTERPRISE 0x00000004
#endif 

#ifndef PRODUCT_ULTIMATE
#define PRODUCT_ULTIMATE 0x00000001
#endif

bool getWindowsVersion(char* version)
{
	int index = 0;
	OSVERSIONINFOEX osvi = {};

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!GetVersionEx((OSVERSIONINFO*)&osvi))
		return false;

	if (osvi.dwPlatformId != VER_PLATFORM_WIN32_NT)
		return false;
// Vista
	if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
	{
		if (osvi.wProductType == VER_NT_WORKSTATION)
		{
			index += sprintf(version + index, "Microsoft Windows Vista");

			uint32_t productType = 0;

			HMODULE hKernel = GetModuleHandle("KERNEL32.DLL");

			if (hKernel)
			{
				typedef bool (*funcGetProductInfo)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t*);
				funcGetProductInfo pGetProductInfo = (funcGetProductInfo)GetProcAddress(hKernel, "GetProductInfo"); 

				if (pGetProductInfo)
					pGetProductInfo(6, 0, 0, 0, &productType);
	  
				switch (productType)
				{
				case PRODUCT_STARTER:
				{
					index += sprintf(version + index, " Starter");
					break;
				}
				case PRODUCT_HOME_BASIC_N:
				{
					index += sprintf(version + index, " Home Basic N");
					break;
				}
				case PRODUCT_HOME_BASIC:
				{
					index += sprintf(version + index, " Home Basic");
					break;
				}
				case PRODUCT_HOME_PREMIUM:
				{
					index += sprintf(version + index, " Home Premium");
					break;
				}
				case PRODUCT_BUSINESS_N:
				{
					index += sprintf(version + index, " Business N");
					break;
				}
				case PRODUCT_BUSINESS:
				{
					index += sprintf(version + index, " Business");
					break;
				}
				case PRODUCT_ENTERPRISE:
				{
					index += sprintf(version + index, " Enterprise");
					break;
				}
				case PRODUCT_ULTIMATE:
				{
					index += sprintf(version + index, " Ultimate");
					break;
				}
				default:
					break;
				}
			}
		}
		else if (osvi.wProductType == VER_NT_SERVER)
		{
			index += sprintf(version + index, "Microsoft Windows Server 2008");

			if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
				index += sprintf(version + index, " Datacenter Edition");
			else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
				index += sprintf(version + index, " Enterprise Edition");
			else if (osvi.wSuiteMask == VER_SUITE_BLADE)
				index += sprintf(version + index, " Web Edition");
			else
				index += sprintf(version + index, " Standard Edition");
		}
	}
// Windows Server 2003
	else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
	{
		index += sprintf(version + index, "Microsoft Windows Server 2003");

		if (GetSystemMetrics(SM_SERVERR2))
			index += sprintf(version + index, " R2");

		if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
			index += sprintf(version + index, " Datacenter Edition");
		else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
			index += sprintf(version + index, " Enterprise Edition");
		else if (osvi.wSuiteMask == VER_SUITE_BLADE)
			index += sprintf(version + index, " Web Edition");
		else
			index += sprintf(version + index, " Standard Edition");
	}
// Windows XP
	else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
	{
		index += sprintf(version + index, "Microsoft Windows XP");

		if (GetSystemMetrics(SM_MEDIACENTER))
			index += sprintf(version + index, " Media Center Edition");
		else if (GetSystemMetrics(SM_STARTER))
			index += sprintf(version + index, " Starter Edition");
		else if (GetSystemMetrics(SM_TABLETPC))
			index += sprintf(version + index, " Tablet PC Edition");
		else if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
			index += sprintf(version + index, " Home Edition");
		else
			index += sprintf(version + index, " Professional");
	}
// Windows 2000
	else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
	{
		index += sprintf(version + index, "Microsoft Windows 2000");

		if (osvi.wProductType == VER_NT_WORKSTATION)
		{
			index += sprintf(version + index, " Professional");
		}
		else if (osvi.wProductType == VER_NT_SERVER)
		{
			if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
				index += sprintf(version + index, " Datacenter Server");
			else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
				index += sprintf(version + index, " Advanced Server");
			else
				index += sprintf(version + index, " Server");
		}
	}
// Windows NT 4
	else if (osvi.dwMajorVersion == 4)
	{
		index += sprintf(version + index, "Microsoft Windows NT 4");

		if (osvi.wProductType == VER_NT_WORKSTATION)
		{
			index += sprintf(version + index, " Workstation");
		}
		else if (osvi.wProductType == VER_NT_SERVER)
		{
			if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
				index += sprintf(version + index, " Server, Enterprise Edition");
			else
				index += sprintf(version + index, " Server");
		}
	}
	else
	{
		index += sprintf(version + index, "Microsoft Windows");
	}

// Service pack and full version info
	if (strlen(osvi.szCSDVersion) > 0)
	{
		index += sprintf(version + index, " %s", osvi.szCSDVersion);
	}

	index += sprintf(version + index, " (%d.%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber & 0xFFFF);

// 64-bit Windows
	bool isWow64 = false;
	HMODULE hKernel = GetModuleHandle("kernel32.dll");

	if (hKernel)
	{
		typedef bool (*funcIsWow64Process)(void*, bool*);  

	    funcIsWow64Process pIsWow64Process = (funcIsWow64Process)GetProcAddress(hKernel, "IsWow64Process"); 

	    if (pIsWow64Process)
	    {
			pIsWow64Process(GetCurrentProcess(), &isWow64);
		}
	}

	if (isWow64)
		index += sprintf(version + index, "; 64-bit");
	else
		index += sprintf(version + index, "; 32-bit");

	index += sprintf(version + index, ")");
	
	return true;
}

#define MONITOR_DEFAULTTONEAREST    0x00000002

void getWorkingArea(uint32_t *width, uint32_t *height)
{
	typedef void* (WINAPI *MonitorFromWindow)(HWND hwnd, uint32_t dwFlags);
	typedef bool (WINAPI *GetMonitorInfo)(void *hMonitor, LPMONITORINFO lpmi);

	static HMODULE user32 = GetModuleHandle("user32.dll");
	static MonitorFromWindow pMonitorFromWindow = (MonitorFromWindow)GetProcAddress(user32, "MonitorFromWindow");
	static GetMonitorInfo pGetMonitorInfo = (GetMonitorInfo)GetProcAddress(user32, "GetMonitorInfoA");

	if (pMonitorFromWindow == NULL)
	{
		RECT rect;

		// Required for NT4 - no multi-monitor support
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

		*width = rect.right - rect.left;
		*height = rect.bottom - rect.top;
	}
	else
	{
		void *hMonitor = pMonitorFromWindow(GetForegroundWindow(), MONITOR_DEFAULTTONEAREST);

		MONITORINFO monitorInfo;

		monitorInfo.cbSize = sizeof(MONITORINFO);
		pGetMonitorInfo(hMonitor, &monitorInfo);

		*width = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
		*height = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;
	}
}
#endif
