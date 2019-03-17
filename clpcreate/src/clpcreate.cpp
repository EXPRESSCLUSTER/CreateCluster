#include <stdio.h>
#include <Windows.h>
#include <MsXml2.h>
#include <comdef.h>
#include <stdlib.h>
#include <shlwapi.h>
#include <errno.h>

#include "clpcreate.h"
#include "../../clpconf/src/clpconfin.h"

/* macro */
#define RELEASE(obj) if ((obj) != NULL) { obj->Release(); (obj) = NULL; }
#define SYSFREE(str) if ((str) != NULL) { SysFreeString(str); (str) = NULL; }

static void *g_hxml = NULL;
static char g_charset[16];


/* prototype */

int
main(
	int argc, 
	char *argv[]
)
{
	HRESULT hr;
	IXMLDOMDocument *xmldoc;
	VARIANT_BOOL success;
	VARIANT var;
	WCHAR wxmlpath[CONF_PATH_LEN];
	ISAXContentHandler *con;
	ISAXXMLReader *rdr;
	IStream *stmfile;
	IMXWriter *wrt;
	int	ret;

	CMD_OPTION cmdopt;

	/* initialize */
	hr = S_OK;
	ret = 0;

	if (!strcmp(argv[1], "init"))
	{
		printf("init\n");
		ret = init();
		goto func_exit;
	}

	/* initialize COM */
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		printf("CoInitializeEx() failed. (hr: %x)\n", hr);
		ret = CONF_ERR_COM;
		goto func_exit;
	}
	try
	{
		/* create IXMLDOMDocument */
		hr = CoCreateInstance(CLSID_DOMDocument, 0, CLSCTX_INPROC_SERVER,
			IID_IXMLDOMDocument, (LPVOID*)&xmldoc);
		if (FAILED(hr))
		{
			ret = CONF_ERR_COM;
			xmldoc = NULL;
			goto func_exit;
		}

		hr = xmldoc->put_async(VARIANT_FALSE);
		if (hr != S_OK)
		{
			ret = CONF_ERR_OTHER;
			goto func_exit;
		}

		/* load XML file */
		ret = MultiByteToWideChar(CP_ACP, 0, ".\\clp.conf", -1, wxmlpath, sizeof(wxmlpath));
		if (ret == 0)
		{
			ret = CONF_ERR_OTHER;
			goto func_exit;
		}

		var.vt = VT_BSTR;
		var.bstrVal = SysAllocString(wxmlpath);
		success = VARIANT_FALSE;
		hr = xmldoc->load(var, &success);
		if (hr != S_OK || !success)
		{
			ret = CONF_ERR_OTHER;
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
		ret = CONF_ERR_COM;
		goto func_exit;
	}
	g_hxml = xmldoc;


	if (!strcmp(argv[1], "add"))
	{
		printf("add\n");
		if (!strcmp(argv[2], "cls"))
		{
			add_cls(argv[3], argv[4], argv[5]);
		}
		else if (!strcmp(argv[2], "srv"))
		{
			add_srv(argv[3], argv[4]);
		}
		else if (!strcmp(argv[2], "ip"))
		{
			add_ip(argv[3], argv[4], argv[5]);
		}
		else if (!strcmp(argv[2], "hb"))
		{
			add_hb(argv[3], argv[4]);
		}
		else if (!strcmp(argv[2], "grp"))
		{
			add_grp(argv[3], argv[4]);
		}
		else if (!strcmp(argv[2], "rsc"))
		{
			add_rsc(argv[3], argv[4], argv[5]);
		}
		else if (!strcmp(argv[2], "rscdep"))
		{
			add_rsc_dep(argv[3], argv[4], argv[5]);
		}
		else if (!strcmp(argv[2], "rscparam"))
		{
			add_rsc_param(argv[3], argv[4], argv[5], argv[6]);
		}
		else if (!strcmp(argv[2], "mon"))
		{
			add_mon(argv[3], argv[4]);
		}
		else if (!strcmp(argv[2], "monparam"))
		{
			add_mon_param(argv[3], argv[4], argv[5], argv[6]);
		}
		else if (!strcmp(argv[2], "objnum"))
		{
			add_obj_num(argv[3]);
		}
		else
		{
		}
	}
	else
	{
		printf("invalid\n");
	}

	try
	{
		/* 保存ファイルのクリエート */
		WCHAR buf[1024] = L".\\clp.conf";
		printf("SHCreateStreamOnFile() starts.\n");
		hr = SHCreateStreamOnFile(buf,
			STGM_READWRITE | STGM_SHARE_DENY_WRITE | STGM_CREATE, &stmfile);
		if (hr != S_OK)
		{
			printf("SHCreateStreamOnFile() failed.\n");
			ret = CONF_ERR_OTHER;
			stmfile = NULL;
			goto func_exit;
		}

		/* IMXWriterの生成 */
		printf("CoCreateInstance() starts.\n");
		hr = CoCreateInstance(CLSID_MXXMLWriter, 0, CLSCTX_INPROC_SERVER,
			IID_IMXWriter, (LPVOID*)&wrt);
		if (hr != S_OK)
		{
			printf("CoCreateInstance() failed.\n");
			ret = CONF_ERR_OTHER;
			wrt = NULL;
			goto func_exit;
		}

		wrt->put_version(_bstr_t("1.0"));
//		wrt->put_encoding(_bstr_t(g_charset));
		/* TODO: need to check charset and change encode */
		wrt->put_encoding(_bstr_t("SJIS"));
		wrt->put_indent(VARIANT_TRUE);
		wrt->put_output(_variant_t((IUnknown*)(IUnknownPtr)stmfile));

		/* SAXXMLReaderの生成 */
		printf("CoCreateInstance(Reader) starts.\n");
		hr = CoCreateInstance(CLSID_SAXXMLReader, 0, CLSCTX_INPROC_SERVER,
			IID_ISAXXMLReader, (LPVOID*)&rdr);
		if (hr != S_OK)
		{
			printf("CoCreateInstance() failed.\n");
			ret = CONF_ERR_OTHER;
			rdr = NULL;
			goto func_exit;
		}

		printf("QueryInterface() starts.\n");
		hr = wrt->QueryInterface(IID_ISAXContentHandler, (void **)&con);
		if (hr != S_OK)
		{
			printf("QueryInterface() failed.\n");
			ret = CONF_ERR_OTHER;
			goto func_exit;
		}

		rdr->putContentHandler(con);

		/* 保存ファイルへ書き込み */
		printf("parse() starts.\n");
		hr = rdr->parse(_variant_t((IUnknown*)(IUnknownPtr)xmldoc));
		if (hr != S_OK)
		{
			printf("parse() failed\n");
			ret = CONF_ERR_OTHER;
			goto func_exit;
		}
	}
	catch (_com_error &e)
	{
#if 0
		log_write(LOG_ERR, "Error = %x\nMessage = %s\nDescription = %s",
			e.Error(), (char*)e.ErrorMessage(), (char*)e.Description());
#endif
		printf("com error\n");
		ret = CONF_ERR_OTHER;
		goto func_exit;
	}
	
	RELEASE(rdr);
	RELEASE(con);
	RELEASE(wrt);
	RELEASE(stmfile);

func_exit:

	return ret;
}


int
init
(
	void
)
{
	create_file();

	return 0;
}


int
add_cls
(
	IN char *clsname,
	IN char *lang,
	IN char *os
)
{
	char path[CONF_PATH_LEN];
	int ret;

	ret = 0;

	strcpy(g_charset, lang);
	sprintf_s(path, CONF_PATH_LEN, "/root/all/charset");
	ret = set_value(g_hxml, path, CONF_CHAR, lang);
	if (ret)
	{
		goto func_exit;
	}
	sprintf_s(path, CONF_PATH_LEN, "/root/all/charset");
	ret = set_value(g_hxml, path, CONF_CHAR, lang);
	if (ret)
	{
		goto func_exit;
	}
	sprintf_s(path, CONF_PATH_LEN, "/root/all/serveros");
	ret = set_value(g_hxml, path, CONF_CHAR, os);
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}
	sprintf_s(path, CONF_PATH_LEN, "/root/all/encode");
	ret = set_value(g_hxml, path, CONF_CHAR, lang);
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}

	/* set cluster name, comment */
	sprintf_s(path, CONF_PATH_LEN, "/root/cluster/name");
	ret = set_value(g_hxml, path, CONF_CHAR, clsname);
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}
	sprintf_s(path, CONF_PATH_LEN, "/root/cluster/comment");
	ret = set_value(g_hxml, path, CONF_CHAR, " ");
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}

func_exit:
	return 0;
}


int
add_srv
(
	IN char *srvname,
	IN char *priority
)
{
	char path[CONF_PATH_LEN];
	int ret;

	ret = 0;

	/* set charset, serveros, encode */
	sprintf_s(path, CONF_PATH_LEN, "/root/server@%s/priority", srvname);
	ret = set_value(g_hxml, path, CONF_CHAR, priority);
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}

func_exit:
	return ret;
}


int
add_ip(
	IN char *srvname,
	IN char *id,
	IN char *ipaddr
)
{
	char path[CONF_PATH_LEN];
	int ret;

	ret = 0;

	/* set charset, serveros, encode */
	sprintf_s(path, CONF_PATH_LEN, "/root/server@%s/device@%s/type", srvname, id);
	ret = set_value(g_hxml, path, CONF_CHAR, "lan");
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}
	sprintf_s(path, CONF_PATH_LEN, "/root/server@%s/device@%s/info", srvname, id);
	ret = set_value(g_hxml, path, CONF_CHAR, ipaddr);
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}

func_exit:
	return ret;


	return 0;
}


int
add_hb(
	IN char *id,
	IN char *priority
)
{
	char path[CONF_PATH_LEN];
	int khbenum = 0;
	int ret;

	/* initialize */
	ret = CONF_ERR_SUCCESS;

	/* set heartbeat to cluster */
	sprintf_s(path, CONF_PATH_LEN, "/root/heartbeat/types@lankhb");
	ret = set_value(g_hxml, path, CONF_CHAR, "");
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}
	khbenum = atoi(priority);
	khbenum++;
	sprintf_s(path, CONF_PATH_LEN, "/root/heartbeat/lankhb@lankhb%d/priority", khbenum);
	ret = set_value(g_hxml, path, CONF_CHAR, priority);
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}
	sprintf_s(path, CONF_PATH_LEN, "/root/heartbeat/lankhb@lankhb%d/device", khbenum);
	ret = set_value(g_hxml, path, CONF_CHAR, id);
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}

	return ret;
}


int
add_grp(
	IN char *grptype,
	IN char *grpname
)
{
	char path[CONF_PATH_LEN];
	int ret;

	/* initialize */
	ret = CONF_ERR_SUCCESS;

	/* add group to cluster */
	sprintf_s(path, CONF_PATH_LEN, "/root/group@%s/type", grpname);
	ret = set_value(g_hxml, path, CONF_CHAR, grptype);
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}
	sprintf_s(path, CONF_PATH_LEN, "/root/group@%s/comment", grpname);
	ret = set_value(g_hxml, path, CONF_CHAR, " ");
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}

	return ret;
}


int
add_rsc(
	IN char *grpname,
	IN char *rsctype,
	IN char *rscname
)
{
	char path[CONF_PATH_LEN];
	int ret;

	/* initialize */
	ret = CONF_ERR_SUCCESS;

	/* add a resource to a group */
	sprintf_s(path, CONF_PATH_LEN, "/root/group@%s/resource@%s@%s", grpname, rsctype, rscname);
	ret = set_value(g_hxml, path, CONF_CHAR, "");
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}

	/* add a resource to a cluster */
	sprintf_s(path, CONF_PATH_LEN, "/root/resource/types@%s", rsctype);
	ret = set_value(g_hxml, path, CONF_CHAR, "");
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}
	sprintf_s(path, CONF_PATH_LEN, "/root/resource/%s@%s/comment", rsctype, rscname);
	ret = set_value(g_hxml, path, CONF_CHAR, " ");
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}

	return ret;
}


int
add_rsc_dep(
	IN char *rsctype,
	IN char *rscname,
	IN char *depend
)
{
	char path[CONF_PATH_LEN];
	int ret;

	/* initialize */
	ret = CONF_ERR_SUCCESS;

	/* add group to cluster */
	sprintf_s(path, CONF_PATH_LEN, "/root/resource/%s@%s/depend@%s", rsctype, rscname, depend);
	ret = set_value(g_hxml, path, CONF_CHAR, "");
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}

	return ret;
}


int
add_rsc_param(
	IN char *rsctype,
	IN char *rscname,
	IN char *tag,
	IN char *param
)
{
	char path[CONF_PATH_LEN];
	int ret;

	/* initialize */
	ret = CONF_ERR_SUCCESS;

	/* add group to cluster */
	sprintf_s(path, CONF_PATH_LEN, "/root/resource/%s@%s/parameters/%s", rsctype, rscname, tag);
	ret = set_value(g_hxml, path, CONF_CHAR, param);
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}

	return ret;
}



/**
 * add_mon
 */
int
add_mon(
	IN char *montype,
	IN char *monname
)
{
	char path[CONF_PATH_LEN];
	int ret;

	/* initialize */
	ret = CONF_ERR_SUCCESS;

	/* add a resource to a group */
	sprintf_s(path, CONF_PATH_LEN, "/root/monitor/types@%s", montype);
	ret = set_value(g_hxml, path, CONF_CHAR, "");
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}

	/* add a resource to a cluster */
	sprintf_s(path, CONF_PATH_LEN, "/root/monitor/%s@%s/comment", montype, monname);
	ret = set_value(g_hxml, path, CONF_CHAR, " ");
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}

	return ret;
}


/**
 * clpconf_add_mon_param
 */
int
add_mon_param(
	IN char *montype,
	IN char *monname,
	IN char *tag,
	IN char *param
)
{
	char path[CONF_PATH_LEN];
	int ret;

	/* initialize */
	ret = CONF_ERR_SUCCESS;

	/* add a resource to a group */
	sprintf_s(path, CONF_PATH_LEN, "/root/monitor/%s@%s/%s", montype, monname, tag);
	ret = set_value(g_hxml, path, CONF_CHAR, param);
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}

	return ret;
}

int add_obj_num(
	IN char *objnum
)
{
	char path[CONF_PATH_LEN];
	int ret;

	/* initialize */
	ret = CONF_ERR_SUCCESS;

	/* add a resource to a group */
	sprintf_s(path, CONF_PATH_LEN, "/root/webmgr/client/objectnumber");
	ret = set_value(g_hxml, path, CONF_CHAR, objnum);
	if (ret)
	{
		printf("save_value() failed. (ret: %d)\n", ret);
		ret = CONF_ERR_FILE;
	}

	return 0;
}