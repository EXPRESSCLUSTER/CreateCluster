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
	nfuncret = clpconf_init("jp", "windows", "");
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

	/* add server to cluster */
	nfuncret = clpconf_add_srv("ws2016-197", "0");
	if (nfuncret)
	{
		printf("clpconf_add_srv() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}
	nfuncret = clpconf_add_srv("ws2016-198", "1");
	if (nfuncret)
	{
		printf("clpconf_add_srv() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}

	/* add IP address to server */
	nfuncret = clpconf_add_ip("ws2016-197", "0", "192.168.0.197");
	if (nfuncret)
	{
		printf("clpconf_add_ip() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}
	nfuncret = clpconf_add_ip("ws2016-197", "1", "192.168.1.197");
	if (nfuncret)
	{
		printf("clpconf_add_ip() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}
	nfuncret = clpconf_add_ip("ws2016-198", "0", "192.168.0.198");
	if (nfuncret)
	{
		printf("clpconf_add_ip() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}
	nfuncret = clpconf_add_ip("ws2016-198", "1", "192.168.1.198");
	if (nfuncret)
	{
		printf("clpconf_add_ip() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}

	/* add a heartbeat to a cluster */
	nfuncret = clpconf_add_hb("0", "0");
	if (nfuncret)
	{
		printf("clpconf_add_ip() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}
	nfuncret = clpconf_add_hb("1", "1");
	if (nfuncret)
	{
		printf("clpconf_add_ip() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}

	/* add NP resource */

	/* add failover group to cluster */
	nfuncret = clpconf_add_grp("failover", "failover");
	if (nfuncret)
	{
		printf("clpconf_add_grp() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}

	/* add a resource to a cluster */
	nfuncret = clpconf_add_rsc("failover", "fip", "fip1");
	if (nfuncret)
	{
		printf("clpconf_add_ip() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}
	nfuncret = clpconf_add_rsc_param("fip", "fip1", "ip", "192.168.1.198");
	if (nfuncret)
	{
		printf("clpconf_add_ip() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}



	/* add monitor resource to cluster */

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
