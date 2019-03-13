/**
 * external functions
 */

#define _WIN32_DCOM
#include <stdio.h>
#include <Windows.h>
#include <MsXml2.h>
#include <comdef.h>
#include <stdlib.h>
#include <shlwapi.h>
#include <errno.h>

#include "clpconf.h"
#include "clpconfin.h"

/* macro */
#define RELEASE(obj) if ((obj) != NULL) { obj->Release(); (obj) = NULL; }
#define SYSFREE(str) if ((str) != NULL) { SysFreeString(str); (str) = NULL; }

/* global variables */
static int g_initcnt = 0;
static BOOL g_cominit = FALSE;
static void *g_hxml = NULL;
static char g_charset[16];
static char g_os[16];

static int g_srvnum = 0;
static int g_hbnum = 0;
static int g_npnum = 0;
static int g_grpnum = 0;
static int g_rscnum = 0;
static int g_monnum = 0;

/**
* clpconf_init
*/
#undef  FUNC
#define FUNC    "_init"

int __stdcall
clpconf_init(
	IN char *lang,
	IN char *os,
	IN char *type
)
{
	HRESULT hr;
	IXMLDOMDocument *xmldoc;
	VARIANT_BOOL success;
	VARIANT var;
	WCHAR wxmlpath[CONF_PATH_LEN];
	int nfuncret, nret;

	/* initialize */
	nfuncret = CONF_ERR_SUCCESS;
	hr = S_OK;

	//
	create_file();

	/* set chareset */
	if (!strcmp(lang, "jp"))
	{
		strcpy(g_charset, "SJIS");
	}
	else if (!strcmp(lang, "cn"))
	{
		strcpy(g_charset, "GB2312");
	}
	else if (!strcmp(lang, "en"))
	{
		strcpy(g_charset, "ASCII");
	}
	else
	{
		printf("invalid lang (jp, en, cn are available only).\n");
		nfuncret = CONF_ERR_FILE;
		goto func_exit;
	}

	/* check OS */
	if (!strcmp(os, "windows"))
	{
		strcpy(g_os, os);
	}
	else
	{
		printf("invalid OS (windows, linux are available only).\n");
		nfuncret = CONF_ERR_FILE;
		goto func_exit;
	}

	/* initialize COM */
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		printf("CoInitializeEx() failed. (hr: %x)\n", hr);
		nfuncret = CONF_ERR_COM;
		goto func_exit;
	}
	g_cominit = TRUE;
	try
	{
		/* create IXMLDOMDocument */
		hr = CoCreateInstance(CLSID_DOMDocument, 0, CLSCTX_INPROC_SERVER,
			IID_IXMLDOMDocument, (LPVOID*)&xmldoc);
		if (FAILED(hr))
		{
			nfuncret = CONF_ERR_COM;
			xmldoc = NULL;
			goto func_exit;
		}

		hr = xmldoc->put_async(VARIANT_FALSE);
		if (hr != S_OK)
		{
			nfuncret = CONF_ERR_OTHER;
			goto func_exit;
		}

		/* load XML file */
		nret = MultiByteToWideChar(CP_ACP, 0, ".\\clp.conf", -1, wxmlpath, sizeof(wxmlpath));
		if (nret == 0)
		{
			nfuncret = CONF_ERR_OTHER;
			goto func_exit;
		}

		var.vt = VT_BSTR;
		var.bstrVal = SysAllocString(wxmlpath);
		success = VARIANT_FALSE;
		hr = xmldoc->load(var, &success);
		if (hr != S_OK || !success)
		{
			nfuncret = CONF_ERR_OTHER;
			VariantClear(&var);
			goto func_exit;
		}
		VariantClear(&var);
	}
	catch (_com_error &e)
	{
#if 0
		log_write(LOG_ERR, "Error = %x\nMessage = %s\nDescription = %s",
			e.Error(), (char*)e.ErrorMessage(), (char*)e.Description());
#endif
		nfuncret = CONF_ERR_COM;
		goto func_exit;
	}

	g_hxml = xmldoc;

func_exit:

	return nfuncret;
}


/**
* clpconf_term
*/
#undef  FUNC
#define FUNC    "_term"

int __stdcall
clpconf_term(
	void
)
{
	int nfuncret;

	/* initialize */
	nfuncret = CONF_ERR_SUCCESS;

	/* uninitialize COM */
	if (g_cominit)
	{
		CoUninitialize();
		g_cominit = FALSE;
	}

	return nfuncret;
}


/**
 * clpconf_save
 */
int __stdcall
clpconf_save(
	void
)
{
	IXMLDOMDocument *xmldoc;
	ISAXContentHandler *con;
	ISAXXMLReader *rdr;
	IStream *stmfile;
	IMXWriter *wrt;
	HRESULT hr;
	char path[CONF_PATH_LEN];
	char obj[CONF_PATH_LEN];
	int objnum;
	int nfuncret;

	/* initialize */
	nfuncret = CONF_ERR_SUCCESS;
	xmldoc = (IXMLDOMDocument *)g_hxml;

	/* calculate object number */
	printf("g_srvnum: %d\n", g_srvnum);
	printf("g_hbnum : %d\n", g_hbnum);
	printf("g_npnum : %d\n", g_npnum);
	printf("g_grpnum: %d\n", g_grpnum);
	printf("g_rscnum: %d\n", g_rscnum);
	printf("g_monnum: %d\n", g_monnum);
	objnum = 4 
			+ g_srvnum 
			+ (g_srvnum * g_hbnum) 
			+ (g_srvnum * g_npnum)
			+ g_grpnum
			+ g_rscnum
			+ g_monnum;
	printf("objnum  : %d\n", objnum);
	itoa(objnum, obj, 10);
	sprintf_s(path, CONF_PATH_LEN, "/root/webmgr/client/objectnumber");
	nfuncret = set_value(g_hxml, path, CONF_CHAR, obj);
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}

	/* set charset, serveros, encode */
	try
	{
		/* 保存ファイルのクリエート */
		WCHAR buf[1024] = L".\\clp.conf";
		//strcpy(buf, ".\\clp.conf");
		LPCWSTR buf2;
		buf2 = buf;
		hr = SHCreateStreamOnFile(buf,
			STGM_READWRITE | STGM_SHARE_DENY_WRITE | STGM_CREATE, &stmfile);
		if (hr != S_OK)
		{
//			log_write(LOG_ERR, "SHCreateStreamOnFile(%s) failed. (ret=%x)", xmlpath, hr);
			nfuncret = CONF_ERR_OTHER;
			stmfile = NULL;
			goto func_exit;
		}

		/* IMXWriterの生成 */
		hr = CoCreateInstance(CLSID_MXXMLWriter, 0, CLSCTX_INPROC_SERVER,
			IID_IMXWriter, (LPVOID*)&wrt);
		if (hr != S_OK)
		{
//			log_write(LOG_ERR, "CoCreateInstance() failed. (ret=%x)", hr);
			nfuncret = CONF_ERR_OTHER;
			wrt = NULL;
			goto func_exit;
		}

		wrt->put_version(_bstr_t("1.0"));
		wrt->put_encoding(_bstr_t(g_charset));
		wrt->put_indent(VARIANT_TRUE);
		wrt->put_output(_variant_t((IUnknown*)(IUnknownPtr)stmfile));

		/* SAXXMLReaderの生成 */
		hr = CoCreateInstance(CLSID_SAXXMLReader, 0, CLSCTX_INPROC_SERVER,
			IID_ISAXXMLReader, (LPVOID*)&rdr);
		if (hr != S_OK)
		{
//			log_write(LOG_ERR, "CoCreateInstance() failed. (ret=%x)", hr);
			nfuncret = CONF_ERR_OTHER;
			rdr = NULL;
			goto func_exit;
		}

		hr = wrt->QueryInterface(IID_ISAXContentHandler, (void **)&con);
		if (hr != S_OK)
		{
//			log_write(LOG_ERR, "QueryInterface() failed. (ret=%x)", hr);
			nfuncret = CONF_ERR_OTHER;
			goto func_exit;
		}

		rdr->putContentHandler(con);

		/* 保存ファイルへ書き込み */
		hr = rdr->parse(_variant_t((IUnknown*)(IUnknownPtr)xmldoc));
		if (hr != S_OK)
		{
//			log_write(LOG_ERR, "parse(%s) failed. (ret=%x)", xmlpath, hr);
			nfuncret = CONF_ERR_OTHER;
			goto func_exit;
		}
	}
	catch (_com_error &e)
	{
#if 0
		log_write(LOG_ERR, "Error = %x\nMessage = %s\nDescription = %s",
			e.Error(), (char*)e.ErrorMessage(), (char*)e.Description());
#endif
		nfuncret = CONF_ERR_OTHER;
		goto func_exit;
	}

func_exit:

	/* 後処理 */
	RELEASE(rdr);
	RELEASE(con);
	RELEASE(wrt);
	RELEASE(stmfile);

	return nfuncret;
}


/**
 * clpconf_add_cls
 */
int __stdcall
clpconf_add_cls(
	IN char *clsname
)
{
	char path[CONF_PATH_LEN];
	int nfuncret;

	/* initialize */
	nfuncret = CONF_ERR_SUCCESS;

	/* set charset, serveros, encode */
	sprintf_s(path, CONF_PATH_LEN, "/root/all/charset");
	nfuncret = set_value(g_hxml, path, CONF_CHAR, g_charset);
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}
	sprintf_s(path, CONF_PATH_LEN, "/root/all/serveros");
	nfuncret = set_value(g_hxml, path, CONF_CHAR, g_os);
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}
	sprintf_s(path, CONF_PATH_LEN, "/root/all/encode");
	nfuncret = set_value(g_hxml, path, CONF_CHAR, g_charset);
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}

	/* set cluster name, comment */
	sprintf_s(path, CONF_PATH_LEN, "/root/cluster/name");
	nfuncret = set_value(g_hxml, path, CONF_CHAR, clsname);
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}
	sprintf_s(path, CONF_PATH_LEN, "/root/cluster/comment");
	nfuncret = set_value(g_hxml, path, CONF_CHAR, " ");
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}

	return nfuncret;
}


/**
 * clpconf_add_srv
 * - Add a server to a cluster
 */
int __stdcall
clpconf_add_srv(
	IN char *srvname,
	IN char *priority
)
{
	char path[CONF_PATH_LEN];
	int nfuncret;

	/* initialize */
	nfuncret = CONF_ERR_SUCCESS;

	/* set charset, serveros, encode */
	sprintf_s(path, CONF_PATH_LEN, "/root/server@%s/priority", srvname);
	nfuncret = set_value(g_hxml, path, CONF_CHAR, priority);
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}
	g_srvnum++;

	return 0;
}

/**
 * add IP address to a server
 */
int __stdcall
clpconf_add_ip(
	IN char *srvname,
	IN char *id,
	IN char *ipaddr
)
{
	char path[CONF_PATH_LEN];
	int nfuncret;

	/* initialize */
	nfuncret = CONF_ERR_SUCCESS;

	/* set IP addres to server */
	sprintf_s(path, CONF_PATH_LEN, "/root/server@%s/device@%s/type", srvname, id);
	nfuncret = set_value(g_hxml, path, CONF_CHAR, "lan");
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}
	sprintf_s(path, CONF_PATH_LEN, "/root/server@%s/device@%s/info", srvname, id);
	nfuncret = set_value(g_hxml, path, CONF_CHAR, ipaddr);
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}

	return nfuncret;
}

/**
 * clpconf_add_hb
 */
int __stdcall
clpconf_add_hb(
	IN char *priority,
	IN char *id
)
{
	char path[CONF_PATH_LEN];
	int khbenum = 0;
	int nfuncret;

	/* initialize */
	nfuncret = CONF_ERR_SUCCESS;

	/* set heartbeat to cluster */
	sprintf_s(path, CONF_PATH_LEN, "/root/heartbeat/types@lankhb");
	nfuncret = set_value(g_hxml, path, CONF_CHAR, "");
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}
	khbenum = atoi(priority);
	khbenum++;
	sprintf_s(path, CONF_PATH_LEN, "/root/heartbeat/lankhb@lankhb%d/priority", khbenum);
	nfuncret = set_value(g_hxml, path, CONF_CHAR, priority);
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}
	sprintf_s(path, CONF_PATH_LEN, "/root/heartbeat/lankhb@lankhb%d/device", khbenum);
	nfuncret = set_value(g_hxml, path, CONF_CHAR, id);
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}
	g_hbnum++;

	return nfuncret;
}


/**
 * add NP
 */
int __stdcall
clpconf_add_np(
	IN char *ipaddr,
	IN char *priority,
	IN char *mdc
)
{
	return 0;
}


/**
 * clpconf_add_grp
 * - Add a failover group to a cluster
 */
int __stdcall
clpconf_add_grp(
	IN char *grptype,
	IN char *grpname
)
{
	char path[CONF_PATH_LEN];
	int nfuncret;

	/* initialize */
	nfuncret = CONF_ERR_SUCCESS;

	/* add group to cluster */
	sprintf_s(path, CONF_PATH_LEN, "/root/group@%s/type", grpname);
	nfuncret = set_value(g_hxml, path, CONF_CHAR, grptype);
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}
	sprintf_s(path, CONF_PATH_LEN, "/root/group@%s/comment", grpname);
	nfuncret = set_value(g_hxml, path, CONF_CHAR, " ");
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}
	g_grpnum++;

	return nfuncret;
}


/**
 * clpconf_add_rsc
 */
int __stdcall
clpconf_add_rsc(
	IN char *grpname,
	IN char *rsctype,
	IN char *rscname
)
{
	char path[CONF_PATH_LEN];
	int nfuncret;

	/* initialize */
	nfuncret = CONF_ERR_SUCCESS;

	/* add a resource to a group */
	sprintf_s(path, CONF_PATH_LEN, "/root/group@%s/resource/%s@%s", grpname, rsctype, rscname);
	nfuncret = set_value(g_hxml, path, CONF_CHAR, "");
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}

	/* add a resource to a cluster */
	sprintf_s(path, CONF_PATH_LEN, "/root/resource/types@%s", rsctype);
	nfuncret = set_value(g_hxml, path, CONF_CHAR, "");
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}
	sprintf_s(path, CONF_PATH_LEN, "/root/resource/%s@%s/comment", rsctype, rscname);
	nfuncret = set_value(g_hxml, path, CONF_CHAR, " ");
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}
	g_rscnum++;

	return nfuncret;
}


/**
 * clpconf_add_rsc_param
 */
int __stdcall
clpconf_add_rsc_param(
	IN char *rsctype,
	IN char *rscname,
	IN char *tag,
	IN char *param
)
{
	char path[CONF_PATH_LEN];
	int nfuncret;

	/* initialize */
	nfuncret = CONF_ERR_SUCCESS;

	/* add group to cluster */
	sprintf_s(path, CONF_PATH_LEN, "/root/resource/%s@%s/parameters/%s", rsctype, rscname, tag);
	nfuncret = set_value(g_hxml, path, CONF_CHAR, param);
	if (nfuncret)
	{
		printf("save_value() failed. (ret: %d)\n", nfuncret);
		nfuncret = CONF_ERR_FILE;
	}

	return nfuncret;
}


/**
 * clpconf_add_mon
 */
int __stdcall
clpconf_add_mon(
	IN char *montype,
	IN char *monname
)
{
	return 0;
}


/**
 * clpconf_add_mon_param
 */
int __stdcall
clpconf_add_mon_param(
	IN char *montype,
	IN char *monname,
	IN char *path,
	IN char *param
)
{
	return 0;
}


