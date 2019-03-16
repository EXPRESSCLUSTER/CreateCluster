/**
 * header file for exteral functions
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

#define CONF_PATH_LEN	1024
#define CONF_CHAR		0
#define CONF_INT		1

/* prototype */
#ifdef __cplusplus
extern "C" {
#endif
	/**
	 * clpconf_init
     *  purpose
	 *   initialize clpconf library 
	 *  argument
	 *   lang: jp, en, cn
	 *   os  : windows, linux
	 *   type: FIXME
	 */
	int __stdcall clpconf_init(IN char *lang, IN char *os, IN char *type);

	/**
	 * clpconf_term
	 *  purpose
	 *   terminate clpconf library
	 *  argument
     *   void
	 */
	int __stdcall clpconf_term(void);

	/**
	 * clpconf_save
	 *  purpose
	 *   save the cluster configuration file
	 *  argument
	 *   void
	 */
	int __stdcall clpconf_save(void);

	/**
	 * clpconf_add_cls
	 *  purpose
	 *   add a cluster
	 *  argument
	 *   clsname: cluster name (must be lower case)
	 */
	int __stdcall clpconf_add_cls(IN char *clsname);

	/**
	 * clpconf_add_srv
	 *  purpose
	 *   add a server to a cluster
	 *  argument
	 *   srvname : server name (must be lower case)
	 *   priority: priority of a server (zero-based indexing)
	 */
	int __stdcall clpconf_add_srv(IN char *srvname, IN char *priority);

	/**
	 * clpconf_add_ip
	 *  purpose
	 *   add IP adress to a server
	 *  argument
	 *   srvname: server name (must be lower case)
	 *   id     : ID of network interface (zero-based indexing)
	 *   ipaddr : IP address (e.g. 192.168.0.1)
	 */
	int __stdcall clpconf_add_ip(IN char *srvname, IN char *id, IN char *ipaddr);

	/**
	 * clpconf_add_hd
	 *  purpose
	 *   add a heartbeat to a cluster
	 *  argument
	 *   priority: priority of network interface (zero-based indexing)
	 *   id      : ID of network interface (zero-based indexing)
	 */
	int __stdcall clpconf_add_hb(IN char *priority, IN char *id);

	/**
	 * clpconf_add_grp
	 *  purpose
	 *   add a group to a cluster
	 *  argument
	 *   grptype: group type
	 *            failover group  : failover
	 *            Management group: cluster
	 *   grpname: group name (e.g. failover, ManagementGroup)
	 */
	int __stdcall clpconf_add_grp(IN char *grptype, IN char *grpname);

	/**
	 * clpconf_add_rsc
	 *  purpose
	 *   add a resource to a group
	 *  argument
	 *   grpname: group name (e.g. failover, ManagementGroup)
	 *   rsctype: resource type (e.g. fip, disk, script)
	 *   rscname: resource name (e.g. fip-192-168-1-199)
	 */
	int __stdcall clpconf_add_rsc(IN char *grpname, IN char *grsctype, IN char *rscname);

	/**
	 * clpconf_add_rsc_param
	 *  purpose
	 *   add a parameter to a resource
	 *  argument
	 *   rsctype: resource type (e.g. fip, disk, script)
	 *   rscname: resource name (e.g. fip-192-168-1-199)
	 *   tag    : tag name (e.g. ip)
	 *   param  : parameter for a resource (e.g. 192.168.1.199)
	 */
	int __stdcall clpconf_add_rsc_param(IN char *rsctype, IN char *rscname, IN char *tag, IN char *param);

	int __stdcall clpconf_add_mon(IN char *montype, IN char *monname);
	int __stdcall clpconf_add_mon_param(IN char *montype, IN char *monname, IN char *tag, IN char *param);

#ifdef __cplusplus
}
#endif
