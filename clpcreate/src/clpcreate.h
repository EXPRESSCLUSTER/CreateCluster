/**
 * usage
 *
 * - initialize
 *   C:\> clpcreate.exe init
 *
 * - add a cluster
 *   C:\> clpcreate.exe add cls <cluster name> <lang> <os> <type>
 *   e.g. C:\> clpcreate.exe add cls mycluster SJIS windows 1
 *
 * - add a server to a cluster
 *   C:\> clpcreate.exe add srv <server name> <priority>
 *   e.g. C:\> clpcreate.exe add srv server1 0
 *        C:\> clpcreate.exe add srv server2 1
 *
 * - add a group to a cluster
 *   C:\> clpcreate.exe add grp <group type> <group name>
 *   e.g. C:\> clpcreate.exe add grp failover failover1
 *        C:\> clpcreate.exe add grp cluster ManagementGroup
 *
 * - add a resource to a group
 *   C:\> clpcreate.exe add rsc <group name> <resource type> <resource name>
 *   e.g. C:\> clpcreate.exe add rsc failover1 fip fip1
 *
 * - add a parameter to a resource
 *   C:\> clpcreate.exe add rsc <resource type> <resource name> <tag> <param>
 *   e.g. C:\> clpcreate.exe add rscparam fip fip1 ip 192.168.1.9
 *        C:\> clpcreate.exe add rscparam service service_ora name OracleServiceSID 
 *
 * - add a monitor to a cluster
 *   C:\> clpcreate.exe add mon <monitor type> <monitor name>
 *   e.g. C:\> clpcreate.exe add fipw fipw1
 *
 * - add a parameter to a monitor 
 *   C:\> clpcreate.exe add mon <monitor type> <monitor name> <tag> <param>
 *   e.g. C:\> clpcreate.exe add monparam userw userw target none
 *        C:\> clpcreate.exe add monparam userw userw relation/type cls
 *        C:\> clpcreate.exe add monparam userw userw relation/name LocalServer
 *
 *        
 */

enum _func_retval__
{
	CONF_ERR_SUCCESS,
	CONF_ERR_FILE,
	CONF_ERR_COM,
	CONF_ERR_PARAM,
	CONF_ERR_NOTEXIST,
	CONF_ERR_OTHER
};


#define CONF_PATH_LEN		1024
#define CONF_CHAR		0
#define CONF_INT		1




/**
 *   1 -  10: 
 *  11 -  99: add an object
 * 101 - 199: delete an object (not used yet)
 */
#define CONF_INIT			1
#define CONF_ADD_CLS		11
#define CONF_ADD_SRV		12
#define CONF_ADD_IP			13
#define CONF_ADD_HB			14
#define CONF_ADD_NP			15
#define CONF_ADD_GRP		16
#define CONF_ADD_RSC		17
#define CONF_ADD_RSC_PARAM	18
#define CONF_ADD_MON		19
#define CONF_ADD_MON_PARAM	20

typedef struct _CMD_OPTION {
	int		option;
	char	tag[CONF_PATH_LEN];
	char	param[CONF_PATH_LEN];
} CMD_OPTION, *PCMD_OPTION;

/**
 * prototype
 */
int init(void);
int add_cls(IN char *clsname, IN char *lang, IN char *os);
int add_srv(IN char *srvname, IN char *priority);
int add_hba(IN char* srvname, IN char* id, IN char* tag, IN char* param);
int add_ip(IN char *srvname, IN char *id, IN char *ipaddr);
int add_hb(IN char *id, IN char *priority);
int add_diskhb(IN char *id, IN char *priority, IN char *dev, IN char *srv1, IN char *srv2);
int add_grp(IN char *grptype, IN char *grpname);
int add_rsc(IN char *grpname, IN char *rsctype, IN char *rscname);
int add_rsc_dep(IN char *rsctype, IN char *rscname, IN char *depend);
int add_rsc_param(IN char *rsctype, IN char *rscname, IN char *tag, IN char *param);
int add_rsc_guid(IN char* rsctype, IN char* rscname, IN char* srvname, IN char* tag, IN char* guid);
int add_mon(IN char *montype, IN char *monname);
int add_mon_param(IN char *montype, IN char *monname, IN char *tag, IN char *param);
int add_pingnp(IN char *id, IN char *priority, IN char *ip, IN char *srv1, IN char *srv2);
int add_disknp(IN char *id, IN char* priority, IN char *letter, IN char *srv1, IN char *srv2);
int add_obj_num(IN char *objnum);
int add_encode(IN char* encode);
