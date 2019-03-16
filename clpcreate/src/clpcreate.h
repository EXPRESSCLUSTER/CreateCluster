#define CONF_PATH 


typedef struct _CMD_OPTION {
	int		option;
	char		tag[];
	char		param[];
} CMD_OPTION, *PCMD_OPTION;
