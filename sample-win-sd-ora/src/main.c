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
	nfuncret = clpconf_add_cls("cluster");
	if (nfuncret)
	{
		printf("clpconf_add_cls() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}

	/* add server */
	nfuncret = clpconf_add_srv("ws2016-11", "0");
	if (nfuncret)
	{
		printf("clpconf_add_srv() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}

	/* add IP address */

	/* add NP resource */

	/* save cluster configuration */
	nfuncret = clpconf_save();
	if (nfuncret)
	{
		printf("clpconf_save() failed. (ret: %d)\n", nfuncret);
	}

func_exit:

	/* terminate clpconf */
	nfuncret = clpconf_term();
	if (nfuncret)
	{
		printf("clpconf_term() failed. (ret: %d)\n", nfuncret);
	}

	return 0;
}
