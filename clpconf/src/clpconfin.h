/**
 * header file for internal functions
 */
#define WK_INT 64

#ifdef __cplusplus
extern "C" {
#endif
	int __stdcall create_file(void);
//	int __stdcall find_value_node(IN IXMLDOMDocument *, IN IXMLDOMNode *, IN char *, IN char *, IN BOOL, OUT IXMLDOMNode **);
	int __stdcall set_value(IN void *xmlhndl, IN char *path, IN int type, IN void *value);
	int __stdcall find_value_node(IN IXMLDOMDocument *xmldoc, IN IXMLDOMNode *node, IN char *path, IN char *curr, IN BOOL force, OUT IXMLDOMNode **target);
	int __stdcall find_child_node(IXMLDOMDocument *, IXMLDOMNode *, char *, char *, IXMLDOMNode **);
	int __stdcall make_child_node(IXMLDOMDocument *, IXMLDOMNode *, char *, char *, IXMLDOMNode **);
#ifdef __cplusplus
}
#endif

