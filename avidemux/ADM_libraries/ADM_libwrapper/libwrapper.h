#ifndef LIBWRAPPER
#define LIBWRAPPER

class ADM_LibWrapper
{
	protected:
		void* hinstLib;
		bool initialised;

	#ifdef CYG_MANGLING
		virtual char* formatMessage(uint32_t msgCode);
	#endif

		ADM_LibWrapper();
		virtual ~ADM_LibWrapper();		
		virtual bool loadLibrary(const char* path);
		virtual void* getSymbol(const char* name);
		virtual bool getSymbols(int symCount, ...);

	public:
		virtual bool isAvailable();
};

#endif
