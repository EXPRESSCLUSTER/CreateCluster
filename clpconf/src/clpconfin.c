/**
 * internal functions
 */
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <errno.h>

#include "clpconf.h"
#include "clpconfin.h"


/**
 * create initial configuration file
 */
int __stdcall
createfile(
	IN char *lang
)
{
	FILE *fp;
	int nfuncret;

	/* initialize */
	nfuncret = CONF_ERR_SUCCESS;

	/* create configuration file */
	fp = fopen(".\\clp.conf", "w+");
	if (fp == NULL)
	{
		printf("fopen() failed. (errno: %d)\n", errno);
		nfuncret = CONF_ERR_FILE;
		goto func_exit;
	}

	/* insert XML declaration */
	if (!strcmp(lang, "jp"))
	{
		fprintf(fp, "<?xml version=\"1.0\" encoding=\"SJIS\"?>\n");
	}
	else if (!strcmp(lang, "cn"))
	{
		fprintf(fp, "<?xml version=\"1.0\" encoding=\"GB2321\"?>\n");
	}
	else if (!strcmp(lang, "en"))
	{
		fprintf(fp, "<?xml version=\"1.0\" encoding=\"ASCII\"?>\n");
	}
	else
	{
		printf("Invalid lang (jp, en, cn are available only).\n");
		nfuncret = CONF_ERR_FILE;
		goto func_exit;
	}
	fprintf(fp, "<root>\n</root>\n");
	fclose(fp);

func_exit:

	return nfuncret;
}
