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
- Add a heartbeat interface to a cluster.
  ```bat
  C:\> clpcreate.exe add hb <id> <priority>

  Example:
  C:\> clpcreate.exe add hb 0 0
  C:\> clpcreate.exe add hb 1 1
  ```
### add np
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

### add rscdep
- Add dependencies to a resource.
  ```bat
  C:\> clpcreate.exe add rscdep <resource type> <resource name #1> <resource name #2>

  Example:
   script1 depends on fip1.
   C:\> clpcreate.exe add rscdep script script1 fip1

   script1 depends on fip1 and sd1.
   C:\> clpcreate.exe add rscdep script script1 fip1
   C:\> clpcreate.exe add rscdep script script1 sd1
  ```

### add rscparam
- Add a parameter to a resource.
  ```bat
  C:\> clpcreate.exe add rscparam <resource type> <resource name> <tag> <parameter>
  
  Example:
   Set floating IP address
   C:\> clpcreate.exe add rscparam fip fip1 ip 192.168.1.9

  ```

### add mon
- Add a monitor to a cluster.
  ```bat
  C:\> clpcreate.exe add mon <monitor type> <monitor name>
  
  Example:
  C:\> clpcreate.exe add mon fipw fipw1
  C:\> clpcreate.exe add mon oraclew oraclew1
  C:\> clpcreate.exe add mon sqlserverw sqlserverw1
  ```

### add monparam
- Add a parameter to a monitor.
  ```bat
  C:\> clpcreate.exe add monparam <monitor type> <monitor name> <tag> <parameter>
  
  Example:
   Monitor timing
    Always
      C:\> clpcreate.exe add monparam fipw fipw1 target none
    Active
      C:\> clpcreate.exe add monparam fipw fipw1 target fip1

   Recovery action
    Restart resource (e.g. fip1)
      C:\> clpcreate.exe add monparam fipw fipw1 relation/type rsc
      C:\> clpcreate.exe add monparam fipw fipw1 relation/name fip1
    Restart resource

  ```

## Samples
### Oracle, SAN

### SQL Server, SAN