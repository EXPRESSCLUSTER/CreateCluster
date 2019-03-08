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

	/* initialize clpconf */
	nfuncret = clpconf_init("jp", "windows");
	if (nfuncret)
	{
		printf("clpconf_init() failed. (ret: %d)\n", nfuncret);
	}

	/* add cluster */
	nfuncret = clpconf_add_cls("windows", "jp", "cluster");


	/* terminate clpconf */
	nfuncret = clpconf_term();
	if (nfuncret)
	{
		printf("clpconf_term() failed. (ret: %d)\n", nfuncret);
	}

	return 0;
}
