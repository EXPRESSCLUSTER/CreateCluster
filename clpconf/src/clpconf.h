/**
 * header file for exteral functions
 */
enum __ohaexml_func_retval__
{
	CONF_ERR_SUCCESS,
	CONF_ERR_FILE,
	CONF_ERR_COM,
	CONF_ERR_PARAM,
	CONF_ERR_NOTEXIST,
	CONF_ERR_OTHER
};

#define CONF_PATH_LEN			1024

#define CONF_CHAR				0
#define CONF_INT				1

#ifdef __cplusplus
extern "C" {
#endif

 /* prototype */
int __stdcall clpconf_init(char *, char *);
int __stdcall clpconf_term(void);
int __stdcall clpconf_add_cls(char *, char *, char *);

#ifdef __cplusplus
}
#endif

