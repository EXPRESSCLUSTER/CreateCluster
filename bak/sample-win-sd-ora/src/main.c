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

	/* add a cluster */
	nfuncret = clpconf_add_cls("cluster");
	if (nfuncret)
	{
		printf("clpconf_add_cls() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}

	/* add a server to a cluster */
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

	/* add an IP address to a server */
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

	/* add a NP resource */


	/* add a failover group to a cluster */
	nfuncret = clpconf_add_grp("failover", "failover");
	if (nfuncret)
	{
		printf("clpconf_add_grp() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}

	/* add a resource to a cluster */
	nfuncret = clpconf_add_rsc("failover", "fip", "fip");
	if (nfuncret)
	{
		printf("clpconf_add_rsc() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}
	nfuncret = clpconf_add_rsc_param("fip", "fip", "ip", "192.168.1.199");
	if (nfuncret)
	{
		printf("clpconf_add_rsc_param() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}

	/* add a monitor resource to a cluster */
	nfuncret = clpconf_add_mon("userw", "userw");
	if (nfuncret)
	{
		printf("clpconf_add_rsc_param() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}
	nfuncret = clpconf_add_mon_param("userw", "userw", "target", "");
	if (nfuncret)
	{
		printf("clpconf_add_rsc_param() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}
	nfuncret = clpconf_add_mon_param("userw", "userw", "relation/type", "cls");
	if (nfuncret)
	{
		printf("clpconf_add_rsc_param() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}
	nfuncret = clpconf_add_mon_param("userw", "userw", "relation/name", "LocalServer");
	if (nfuncret)
	{
		printf("clpconf_add_rsc_param() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}

	nfuncret = clpconf_add_mon("fipw", "fipw");
	if (nfuncret)
	{
		printf("clpconf_add_rsc_param() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}
	nfuncret = clpconf_add_mon_param("fipw", "fipw", "target", "fip");
	if (nfuncret)
	{
		printf("clpconf_add_rsc_param() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}
	nfuncret = clpconf_add_mon_param("fipw", "fipw", "relation/type", "rsc");
	if (nfuncret)
	{
		printf("clpconf_add_rsc_param() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}
	nfuncret = clpconf_add_mon_param("fipw", "fipw", "relation/name", "fip");
	if (nfuncret)
	{
		printf("clpconf_add_rsc_param() failed. (ret: %d)\n", nfuncret);
		goto func_exit;
	}



	/* save a cluster configuration */
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
