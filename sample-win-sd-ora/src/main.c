#include <windows.h>
#include <stdio.h>

#include "../../clpconf/src/clpconf.h"


#define ERR_SUCCESS		0
#define ERR_WIN32API	1

HMODULE			hDll;

int
main(
	void
)
{
	int nfuncret;

	/* initialize */
	nfuncret = ERR_SUCCESS;

	hDll = LoadLibrary("clpconf.dll");
	if (!hDll)
	{
		printf("LoadLibrary() failed (ret: %d)", GetLastError());
		return ERR_WIN32API;
	}


#if 0
	nfuncret = clpconf_init();
	if (!nfuncret)
	{
		printf("clpconf_init() failed. (ret: %d)", nfuncret);
	}

	nfuncret = clpconf_term();
	if (!nfuncret)
	{
		printf("clpconf_term() failed. (ret: %d)", nfuncret);
	}
#endif
	return 0;
}
