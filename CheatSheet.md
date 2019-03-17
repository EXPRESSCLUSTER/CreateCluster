# Cheat Sheet
- This is a cheat sheet to use clpcreate command.

## Options
### init
- Initialize a cluster configuration file (clp.conf).
  ```bat
  C:\> clpcreate.exe init
  ```
### add cls
- Add a cluster to an initial configuration.
  ```bat
  C:\> clpcreate.exe add cls <cluster name> <charactor code> <type>
  
  Example:
  C:\> clpcreate.exe add cls mycluster SJIS 1
  
  <charactor code>
  SJIS  : Japanese
  ASCII : English or other language
  GB2312: Chinese

  <type>
  FIXME
  1:
  ```
### add srv
- Add a server to a cluster.
  ```bat
  C:\> clpcreate.exe add srv <server name> <priority>
  
  Example:
  C:\> clpcreate.exe add srv server1 0
  C:\> clpcreate.exe add srv server2 1

  <priority>
  Zero-based indexing. server1 is the highest priority server on the above example.
  ```
### add ip
- Add IP address to a server.
  ```bat
  C:\> clpcreate.exe add ip <server name> <id> <IP address>
  
  Example:
  C:\> clpcreate.exe add ip server1 0 192.168.0.1
  C:\> clpcreate.exe add ip server1 1 192.168.1.1
  C:\> clpcreate.exe add ip server2 0 192.168.0.2
  C:\> clpcreate.exe add ip server2 1 192.168.1.2

  <id>
  Zero-based indexing.
  ```
### add hb
- FIXME
### add grp
- Add a group to a cluster.
  ```bat
  C:\> clpcreate.exe add grp <group type> <group name>
  
  Example:
  C:\> clpcreate.exe add grp failover failover1
  C:\> clpcreate.exe add grp cluster ManagementGroup

  <group name>
  failover: for general purpose group
  cluster : for Management Group only
  ```
### add rsc
- Add a resource to a group.
  ```bat
  C:\> clpcreate.exe add rsc <group name> <resource type> <resource name>
  
  Example:
  C:\> clpcreate.exe add rsc failover1 fip fip1
  ```

### add rscparam
- FIXME

### add mon

### add monparam
- FIXME

## Samples
### Oracle, SAN
### SQL Server, SAN