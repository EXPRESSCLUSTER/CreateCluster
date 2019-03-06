#define _WIN32_DCOM
#include <stdio.h>
#include <windows.h>
#include <msxml2.h>
#include <comdef.h>
#include <stdlib.h>
#include <shlwapi.h>

#include "clpconf.h"

/* macro */
#define RELEASE(obj) if ((obj) != NULL) { obj->Release(); (obj) = NULL; }
#define SYSFREE(str) if ((str) != NULL) { SysFreeString(str); (str) = NULL; }

/* critical section */
static CRITICAL_SECTION g_critsec;

/* init count */
static int g_initcnt = 0;

/* com init flag */
static BOOL g_cominit = FALSE;

/* internal function */
static int find_value_node(IXMLDOMDocument *, IXMLDOMNode *, char *, char *, BOOL, IXMLDOMNode **);
static int find_child_node(IXMLDOMDocument *, IXMLDOMNode *, char *, char *, IXMLDOMNode **);
static int make_child_node(IXMLDOMDocument *, IXMLDOMNode *, char *, char *, IXMLDOMNode **);

/**
 * DllEntryPoint
 *
 */
#undef  FUNC
#define FUNC    "DllEntryPoint"

BOOL WINAPI
DllEntryPoint(HINSTANCE dll, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		InitializeCriticalSection(&g_critsec);
		break;
	case DLL_PROCESS_DETACH:
		DeleteCriticalSection(&g_critsec);
		break;
	default:
		break;
	}

	return TRUE;
}


/**
* clpconf_init
*/
#undef  FUNC
#define FUNC    "_init"

int __stdcall
clpconf_init(
	void
)
{
	HRESULT hr;
	int nfuncret;

	/* initialize */
	nfuncret = CONF_ERR_SUCCESS;

	EnterCriticalSection(&g_critsec);

	/* check call count */
	if (g_initcnt < 0)
	{
//		nfuncret = OHAE_XMLERR_OTHER;
		goto func_exit;
	}
	else if (g_initcnt == 0)
	{
	}

	/* initialize COM */
	if (!g_cominit)
	{
		hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (FAILED(hr))
		{
		}
		else
		{
			g_cominit = TRUE;
		}
	}

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

	EnterCriticalSection(&g_critsec);

	/* check call count */
	if (g_initcnt <= 0)
	{
//		nfuncret = OHAE_XMLERR_NOT_INIT;
		goto func_exit;
	}
	else if (g_initcnt > 1)
	{
		g_initcnt--;
		goto func_exit;
	}

	/* uninitialize COM */
	if (g_cominit)
	{
		CoUninitialize();
		g_cominit = FALSE;
	}

	g_initcnt--;

func_exit:

	LeaveCriticalSection(&g_critsec);

	return nfuncret;
}


/**
 * clpconf_add_cluster
 */


/**
 * clpconf_set_value
 */
#undef  FUNC
#define FUNC    "_set_value"

int __stdcall
clpconf_set_value(
	IN void *xmlhndl,
	IN char *path,
	IN int type,
	IN void *value
)
{
	IXMLDOMDocument *xmldoc;
	IXMLDOMNode *target;
	IXMLDOMNode *node;
	HRESULT hr;
	char wk_path[CONF_PATH_LEN];
	char wk_int[64];
	void *valp;
	int nfuncret, nret;

	/* initialize */
	nfuncret = CONF_ERR_SUCCESS;
	xmldoc = (IXMLDOMDocument *)xmlhndl;
	target = NULL;
	node = NULL;

	/* check */
	if (g_initcnt <= 0)
	{
		nfuncret = CONF_ERR_NOT_INIT;
		goto func_exit;
	}

	/* check */
	if (xmlhndl == NULL)
	{
//		log_write(LOG_ERR, "xmlhndl is NULL.");
		nfuncret =CONF_ERR_INVALID_PARAM;
		goto func_exit;
	}
	if (path == NULL)
	{
//		log_write(LOG_ERR, "path is NULL.");
		nfuncret = CONF_ERR_INVALID_PARAM;
		goto func_exit;
	}
	if (value == NULL)
	{
//		log_write(LOG_ERR, "value is NULL.");
		nfuncret = CONF_ERR_INVALID_PARAM;
		goto func_exit;
	}

	/* check length */
	if (strlen(path) >= sizeof(wk_path))
	{
//		log_write(LOG_ERR, "path is too long.");
		nfuncret = CONF_ERR_INVALID_PARAM;;
		goto func_exit;
	}

	/* �p�X�̃`�F�b�N */
	if (strncmp(path, "/root/", strlen("/root/")) != 0)
	{
//		log_write(LOG_ERR, "path is invalid. (path=%s)", path);
		nfuncret = CONF_ERR_INVALID_PARAM;
		goto func_exit;
	}

	/* �^�C�v�̃`�F�b�N */
	switch (type) {
	case CONF_INT:
		sprintf(wk_int, "%d", *((int *)value));
		valp = wk_int;
		break;
	case CONF_CHAR:
		valp = value;
		break;
	default:
//		log_write(LOG_ERR, "type is invalid. (type=%d)", type);
		nfuncret = CONF_ERR_INVALID_PARAM;
		goto func_exit;
	}

	try
	{
		/* root�m�[�h���擾 */
		strcpy(wk_path, "/root");
		hr = xmldoc->selectSingleNode(_bstr_t(wk_path), &node);
		if (hr != S_OK)
		{
//			log_write(LOG_ERR, "selectSingleNode(%s) failed. (ret=%x)", wk_path, hr);
			nfuncret = CONF_ERR_OTHER;
			node = NULL;
			goto func_exit;
		}

		/* �^�[�Q�b�g�m�[�h���擾 */
		nret = find_value_node(xmldoc, node, path, wk_path, TRUE, &target);
		if (nret != CONF_ERR_SUCCESS)
		{
			nfuncret = nret;
			target = NULL;
			goto func_exit;
		}

		/* �^�[�Q�b�g�m�[�h�ɐݒ� */
		hr = target->put_text(_bstr_t((char *)valp));
		if (hr != S_OK)
		{
//			log_write(LOG_ERR, "put_text(%s) failed. (ret=%x)", valp, hr);
			nfuncret = CONF_ERR_OTHER;
			goto func_exit;
		}
	}
	catch (_com_error &e)
	{
//		log_write(LOG_ERR, "Error = %x\nMessage = %s\nDescription = %s",
//			e.Error(), (char*)e.ErrorMessage(), (char*)e.Description());
		nfuncret = CONF_ERR_OTHER;
		goto func_exit;
	}

func_exit:

	/* �㏈�� */
	RELEASE(target);
	RELEASE(node);

	if (nfuncret != CONF_ERR_SUCCESS)
	{
//		log_write(LOG_ERR, "%s(%s) failed. (ret=%d)", FUNC, LOG_STR(path), nfuncret);
	}

//	FUNC_LEAVE_INT(nfuncret);
	return nfuncret;
}


static int
find_value_node(
	IN IXMLDOMDocument *xmldoc,
	IN IXMLDOMNode *node,
	IN char *path,
	IN char *curr,
	IN BOOL force,
	OUT IXMLDOMNode **target
)
{
	char element[CONF_PATH_LEN];
	char attribute[CONF_PATH_LEN];
	char *p;
	int nfuncret, nret;

	/* initialize */
	nfuncret = CONF_ERR_SUCCESS;

	strcpy(element, &path[strlen(curr) + 1]);
	strcpy(attribute, "");

	p = strchr(element, '/');
	if (p != NULL)
	{
		*p = '\0';
	}

	p = strchr(element, '@');
	if (p != NULL)
	{
		*p = '\0';
		strcpy(attribute, ++p);
	}

	/* �`���C���h�m�[�h������ */
	nret = find_child_node(xmldoc, node, element, attribute, target);

	if (nret != CONF_ERR_SUCCESS) {

		/* ������Ȃ���� */
		if (!force || nret != CONF_ERR_NOTEXIST_NODE)
		{
			nfuncret = nret;
			goto func_exit;
		}

		/* �`���C���h�m�[�h���쐬 */
		nret = make_child_node(xmldoc, node, element, attribute, target);

		if (nret != CONF_ERR_SUCCESS)
		{
			nfuncret = nret;
			goto func_exit;
		}
	}

	/* �X�ɑ�����H */
	strcat(curr, "/");
	strcat(curr, element);

	if (strlen(attribute) > 0) {
		strcat(curr, "@");
		strcat(curr, attribute);
	}

	if (strcmp(path, curr) != 0)
	{
		node = *target;
		nfuncret = find_value_node(xmldoc, node, path, curr, force, target);
		RELEASE(node);
		goto func_exit;
	}

func_exit:

	/* �㏈�� */

//	FUNC_LEAVE_INT(nfuncret);
	return nfuncret;
}


static int
find_child_node(
	IN IXMLDOMDocument *xmldoc,
	IN IXMLDOMNode *node,
	IN char *element,
	IN char *attribute,
	OUT IXMLDOMNode **child
)
{
	IXMLDOMNodeList *child_list;
	IXMLDOMElement *child_elem;
	IXMLDOMNode *child_node;
	VARIANT attr;
	HRESULT hr;
	BSTR elem;
	long length;
	long i;
	int nfuncret;

//	FUNC_ENTER();

	/* ������ */
	nfuncret = CONF_ERR_SUCCESS;
	child_list = NULL;
	child_node = NULL;
	child_elem = NULL;
	elem = NULL;
	VariantInit(&attr);

	try
	{
		/* �`���C���h�m�[�h���X�g���擾 */
		hr = node->get_childNodes(&child_list);
		if (hr != S_OK)
		{
//			log_write(LOG_ERR, "get_childNodes() failed. (ret=%x)", hr);
			nfuncret = CONF_ERR_OTHER;
			child_list = NULL;
			goto func_exit;
		}

		/* �`���C���h�m�[�h�����擾 */
		hr = child_list->get_length(&length);
		if (hr != S_OK)
		{
//			log_write(LOG_ERR, "get_length() failed. (ret=%x)", hr);
			nfuncret = CONF_ERR_OTHER;
			goto func_exit;
		}

		/* �`���C���h�m�[�h�������J��Ԃ� */
		for (i = 0; i < length; i++)
		{
			RELEASE(child_node);
			RELEASE(child_elem);
			SYSFREE(elem);

			/* �`���C���h�m�[�h���擾 */
			hr = child_list->get_item(i, &child_node);
			if (hr != S_OK)
			{
//				log_write(LOG_ERR, "get_item() failed. (ret=%x)", hr);
				nfuncret = CONF_ERR_OTHER;
				child_node = NULL;
				goto func_exit;
			}

			/* �`���C���h�m�[�h�̃G�������g�����擾 */
			hr = child_node->get_nodeName(&elem);
			if (hr != S_OK)
			{
//				log_write(LOG_ERR, "get_nodeName() failed. (ret=%x)", hr);
				nfuncret = CONF_ERR_OTHER;
				elem = NULL;
				goto func_exit;
			}

			/* �G�������g�����`�F�b�N */
			if (strcmp(_bstr_t(elem), element) != 0)
			{
				/* �s��v */
				continue;
			}

			if (strlen(attribute) > 0)
			{
				/* �`���C���h�m�[�h�̃A�g���r���[�g���擾 */
				hr = child_node->QueryInterface(IID_IXMLDOMElement, (void **)&child_elem);
				if (hr != S_OK)
				{
//					log_write(LOG_ERR, "QueryInterface() failed. (ret=%x)", hr);
					nfuncret = CONF_ERR_OTHER;
					child_elem = NULL;
					goto func_exit;
				}

				hr = child_elem->getAttribute(
					_bstr_t(strcmp(element, "device") == 0 ? "id" : "name"), &attr);

				/* �A�g���r���[�g���`�F�b�N */
				if (hr != S_OK)
				{
					/* �s��v */
					continue;
				}

				if (strcmp(_bstr_t(attr), attribute) != 0)
				{
					/* �s��v */
					VariantClear(&attr);
					continue;
				}

				VariantClear(&attr);
			}

			/* ��v */
			*child = child_node;
			break;
		}

		/* �����H */
		if (i >= length)
		{
			nfuncret = CONF_ERR_NOTEXIST_NODE;
			goto func_exit;
		}
	}
	catch (_com_error &e)
	{
/*
		log_write(LOG_ERR, "Error = %x\nMessage = %s\nDescription = %s",
			e.Error(), (char*)e.ErrorMessage(), (char*)e.Description());
			*/
		nfuncret = CONF_ERR_OTHER;
		goto func_exit;
	}

func_exit:

	/* �㏈�� */
	RELEASE(child_list);
	RELEASE(child_elem);
	SYSFREE(elem);

	if (nfuncret != CONF_ERR_SUCCESS)
	{
		RELEASE(child_node);

		if (child != NULL)
		{
			*child = NULL;
		}
	}

//	FUNC_LEAVE_INT(nfuncret);
	return nfuncret;
}


static int
make_child_node(
	IN IXMLDOMDocument *xmldoc,
	IN IXMLDOMNode *node,
	IN char *element,
	IN char *attribute,
	OUT IXMLDOMNode **child
)
{
	IXMLDOMElement *child_elem;
	IXMLDOMNode *new_child_node;
	VARIANT var;
	HRESULT hr;
	WCHAR wattribute[CONF_PATH_LEN];
	int nfuncret, nret;

//	FUNC_ENTER();

	/* ������ */
	nfuncret = CONF_ERR_SUCCESS;
	VariantInit(&var);
	new_child_node = NULL;
	child_elem = NULL;

	try
	{
		/* �V�K�G�������g�̍쐬 */
		hr = xmldoc->createElement(_bstr_t(element), &child_elem);
		if (hr != S_OK)
		{
//			log_write(LOG_ERR, "createElement() failed. (ret=%x)", hr);
			nfuncret = CONF_ERR_OTHER;
			child_elem = NULL;
			goto func_exit;
		}

		/* �A�g���r���[�g�w�肪����� */
		if (strlen(attribute) > 0)
		{
			/* �A�g���r���[�g���Z�b�g */
			nret = MultiByteToWideChar(CP_ACP, 0, attribute, -1, wattribute, sizeof(wattribute));
			if (nret == 0)
			{
//				log_write(LOG_ERR, "MultiByteToWideChar() failed. (ret=%x)", GetLastError());
				nfuncret = CONF_ERR_OTHER;
				goto func_exit;
			}

			var.vt = VT_BSTR;
			var.bstrVal = SysAllocString(wattribute);

			hr = child_elem->setAttribute(
				_bstr_t(strcmp(element, "device") == 0 ? "id" : "name"), var);

			if (hr != S_OK)
			{
//				log_write(LOG_ERR, "setAttribute() failed. (ret=%x)", hr);
				nfuncret = CONF_ERR_OTHER;
				VariantClear(&var);
				goto func_exit;
			}

			VariantClear(&var);
		}

		/* �`���C���h�m�[�h�̍쐬 */
		hr = node->appendChild(child_elem, &new_child_node);
		if (hr != S_OK)
		{
//			log_write(LOG_ERR, "appendChild() failed. (ret=%x)", hr);
			nfuncret = CONF_ERR_OTHER;
			new_child_node = NULL;
			goto func_exit;
		}
	}
	catch (_com_error &e)
	{
//		log_write(LOG_ERR, "Error = %x\nMessage = %s\nDescription = %s",
//			e.Error(), (char*)e.ErrorMessage(), (char*)e.Description());
		nfuncret = CONF_ERR_OTHER;
		goto func_exit;
	}

	*child = new_child_node;

func_exit:

	/* �㏈�� */
	RELEASE(child_elem);

	if (nfuncret != CONF_ERR_SUCCESS)
	{
		RELEASE(new_child_node);

		if (child != NULL)
		{
			*child = NULL;
		}
	}

//s	FUNC_LEAVE_INT(nfuncret);
	return nfuncret;
}

	int find_value_node(IXMLDOMDocument *, IXMLDOMNode *, char *, char *, BOOL, IXMLDOMNode **)
	{
		return 0;
	}