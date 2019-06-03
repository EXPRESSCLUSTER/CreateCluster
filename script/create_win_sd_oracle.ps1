##################################################
# Sample script for Windows
##################################################
# Parameters
#-------------------------------------------------
#
# $type
#  Set type for final action of monitor resources.
#  0: use default final action (do nothing)
#  1: ignore timeout
#  2: initiate BSoD/panic immediately
$type = 1
# $cluster
#  
$cluster = @{name = "cluster"; encode = "SJIS"; os = "windows"}
#
# $server
#  Top of the list is master node.
$server = @(@("ws2016-197"),
            @("ws2016-198"),
            @())
# 
# $ip
#  Set IP addresses for the servers.
#  On this case, upper IP addresses for server1 and bottom IP addresses for server2.
$ip = @(@("192.168.0.197", "192.168.1.197"),
        @("192.168.0.198", "192.168.1.198"),
        @())
#
# $hb
#  Left : 
#  Right: 
$hb = @(@("0", "0"),
        @("1", "1"),
        @())
#
# $disknp
#
$disknp = @(@("pingnp1", "0", "10200", "0", "0", "192.168.137.1"),
            @())
#
# $pingnp
#
$pingnp = @(@("pingnp1", "0", "10200", "0", "0", "192.168.1.144"),
            @())
#
# $group
#
$group = @(@("failover-oracle"),
           @())
#
$resource = @(@(@("fip","fip", @("ip", "192.168.1.199")), 
                @("sd", "sd", @("volumemountpoint", "S:")),
                @("service", "service-db", @("name", "OracleServiceSID")),
                @("service", "service-listener", @("name", "OracleOraDB12Home1TNSListener")),
                @("script", "script-db")),
              @())
$rscdepend = @(@("fip", "sd"),
               @("sd", "service-db"),
               @("service-db", "service-listener"),
               @("service-listener", "script-db"),
               @())
$monitor = @(@("userw", "userw"),
             )

<#
.\clpcreate.exe add mon userw userw
.\clpcreate.exe add mon ipw ipw-gw
.\clpcreate.exe add mon fipw fipw
.\clpcreate.exe add mon servicew servicew-db
.\clpcreate.exe add mon servicew servicew-listener
.\clpcreate.exe add mon oraclew oraclew
#>
##################################################


##################################################
# You don't need to change the following lines, maybe.
#
# initialize
.\.\clpcreate.exe init

# add a cluster to initial configuration
.\.\clpcreate.exe add cls $cluster["name"] $cluster["encode"] $cluster["os"]

# add a server to a cluster
for ($i = 0; $i -lt ($server.Length - 1); $i++) 
{
    .\clpcreate.exe add srv $server[$i] $i
}

# add an IP address to a server
for ($i = 0; $i -lt ($server.Length - 1); $i++) 
{
    for ($j = 0; $j -lt $ip[$i].Length; $j++) 
    {
        .\clpcreate.exe add ip $server[$i] $j $ip[$i][$j]
    }
}

# add a heartbeat interface to a cluster
for ($i = 0; $i -lt ($hb.Length - 1); $i++) 
{
    .\clpcreate.exe add hb $hb[$i][0] $hb[$i][1]  
}


# add a HBA to a server
.\clpcreate.exe add hba ws2016-197 0 portnumber 3
.\clpcreate.exe add hba ws2016-197 0 deviceid ROOT\ISCSIPRT
.\clpcreate.exe add hba ws2016-197 0 instanceid 0000
.\clpcreate.exe add hba ws2016-198 0 portnumber 3
.\clpcreate.exe add hba ws2016-198 0 deviceid ROOT\ISCSIPRT
.\clpcreate.exe add hba ws2016-198 0 instanceid 0000

# add a network partition resource to a cluster
.\clpcreate.exe add np disknp disknp1 0 10100 
.\clpcreate.exe add np pingnp pingnp1 1 10200 0 0 192.168.1.144

# add a network partition to a server
.\clpcreate.exe add npsrv disknp ws2016-197 10100 019056a3-a7ad-4de3-9ed8-d5e752e501ea R:\
.\clpcreate.exe add npsrv disknp ws2016-198 10100 019056a3-a7ad-4de3-9ed8-d5e752e501ea R:\
.\clpcreate.exe add npsrv pingnp ws2016-197 10200 1
.\clpcreate.exe add npsrv pingnp ws2016-198 10200 1

# add a group to a cluster
for ($i = 0; $i -lt ($group.Length - 1); $i++) 
{
    .\clpcreate.exe add grp failover $group[$i]
}

# add a resouce to a group
for ($i = 0; $i -lt ($resource.Length - 1); $i++) 
{
    for ($j = 0; $j -lt $resource[$i].Length; $j++) 
    {
        .\clpcreate.exe add rsc $group[$i] $resource[$i][$j][0] $resource[$i][$j][1]
        for ($k = 2; $k -lt $resource[$i][$j].Length; $k++) 
        {
            .\clpcreate.exe add rscparam $resource[$i][$j][0] $resource[$i][$j][1] $resource[$i][$j][$k][0] $resource[$i][$j][$k][1]
        }
    }
}

# add a resource dependence
for ($i = 0; $i -lt ($rscdepend.Length - 1); $i++) 
{
    Write-Output "rscdepend:" $rscdepend[$i][1]
    for ($j = 0; $j -lt ($resource.Length - 1); $j++) 
    {
        for ($k = 0; $k -lt $resource[$j].Length; $k++)
        {
            if ($rscdepend[$i][1] -eq $resource[$j][$k][1])
            {
                .\clpcreate.exe add rscdep $resource[$j][$k][0] $resource[$j][$k][1] $rscdepend[$i][0]
            }
        }
    }
}


##################################################
# add GUID to a resource
.\clpcreate.exe add rscguid sd sd ws2016-197 volumeguid 205bce9a-e322-442d-af11-776f8b6af913
.\clpcreate.exe add rscguid sd sd ws2016-198 volumeguid 205bce9a-e322-442d-af11-776f8b6af913
##################################################

##################################################
# add a monitor to a cluster
.\clpcreate.exe add mon userw userw
.\clpcreate.exe add mon ipw ipw-gw
.\clpcreate.exe add mon fipw fipw
.\clpcreate.exe add mon servicew servicew-db
.\clpcreate.exe add mon servicew servicew-listener
.\clpcreate.exe add mon oraclew oraclew
##################################################

##################################################
# add a parameter to a monitor
# ----------------------------------------
# add a parameter to a monitor (user space monitor)
.\clpcreate.exe add monparam userw userw relation/type cls
.\clpcreate.exe add monparam userw userw relation/name LocalServer

# add a parameter to a monitor (IP monitor)
.\clpcreate.exe add monparam ipw ipw-gw relation/type grp
.\clpcreate.exe add monparam ipw ipw-gw relation/name failover-oracle
.\clpcreate.exe add monparam ipw ipw-gw parameters/list@1/ip 192.168.1.144
if ($type -eq 0)
{
    .\clpcreate.exe add monparam ipw ipw-gw emergency/threshold/restart 0
}
elseif ($type -eq 1)
{
    .\clpcreate.exe add monparam ipw ipw-gw emergency/threshold/restart 0
    .\clpcreate.exe add monparam ipw ipw-gw emergency/timeout/notreconfirmation/use 1
    .\clpcreate.exe add monparam ipw ipw-gw emergency/timeout/notrecovery/use 1
}
elseif ($type -eq 2)
{
    .\clpcreate.exe add monparam ipw ipw-gw relation/type cls
    .\clpcreate.exe add monparam ipw ipw-gw relation/name LocalServer
    .\clpcreate.exe add monparam ipw ipw-gw emergency/action 6
}

# add a parameter to a monitor (floating IP monitor)
.\clpcreate.exe add monparam fipw fipw target fip
.\clpcreate.exe add monparam fipw fipw relation/type grp
.\clpcreate.exe add monparam fipw fipw relation/name failover-oracle
.\clpcreate.exe add monparam fipw fipw parameters/monmii 1
if ($type -eq 0)
{
    .\clpcreate.exe add monparam fipw fipw emergency/threshold/restart 0
}
elseif ($type -eq 1)
{
    .\clpcreate.exe add monparam fipw fipw emergency/threshold/restart 0
    .\clpcreate.exe add monparam fipw fipw emergency/timeout/notreconfirmation/use 1
    .\clpcreate.exe add monparam fipw fipw emergency/timeout/notrecovery/use 1
}
elseif ($type -eq 2)
{
    .\clpcreate.exe add monparam fipw fipw relation/type cls
    .\clpcreate.exe add monparam fipw fipw relation/name LocalServer
    .\clpcreate.exe add monparam fipw fipw emergency/action 6
}

# add a parameter to a monitor (service monitor for Oracle DB)
.\clpcreate.exe add monparam servicew servicew-db target service-db
.\clpcreate.exe add monparam servicew servicew-db relation/type grp
.\clpcreate.exe add monparam servicew servicew-db relation/name failover-oracle
if ($type -eq 0)
{
    .\clpcreate.exe add monparam servicew servicew-db emergency/threshold/restart 0
}
elseif ($type -eq 1)
{
    .\clpcreate.exe add monparam servicew servicew-db emergency/threshold/restart 0
    .\clpcreate.exe add monparam servicew servicew-db emergency/timeout/notreconfirmation/use 1
    .\clpcreate.exe add monparam servicew servicew-db emergency/timeout/notrecovery/use 1
}
elseif ($type -eq 2)
{
    .\clpcreate.exe add monparam servicew servicew-db relation/type cls
    .\clpcreate.exe add monparam servicew servicew-db relation/name LocalServer
    .\clpcreate.exe add monparam servicew servicew-db emergency/action 6
}

# add a parameter to a monitor (service monitor for Oracle Listener)
.\clpcreate.exe add monparam servicew servicew-listener target service-listener
.\clpcreate.exe add monparam servicew servicew-listener relation/type grp
.\clpcreate.exe add monparam servicew servicew-listener relation/name failover-oracle
if ($type -eq 0)
{
    .\clpcreate.exe add monparam servicew servicew-listener emergency/threshold/restart 0
}
elseif ($type -eq 1)
{
    .\clpcreate.exe add monparam servicew servicew-listener emergency/threshold/restart 0
    .\clpcreate.exe add monparam servicew servicew-listener emergency/timeout/notreconfirmation/use 1
    .\clpcreate.exe add monparam servicew servicew-listener emergency/timeout/notrecovery/use 1
} 
elseif ($type -eq 2)
{
    .\clpcreate.exe add monparam servicew servicew-listener relation/type cls
    .\clpcreate.exe add monparam servicew servicew-listener relation/name LocalServer
    .\clpcreate.exe add monparam servicew servicew-listener emergency/action 6
}

# add a parameter to a monitor (Oracle monitor)
.\clpcreate.exe add monparam oraclew oraclew target script-db
.\clpcreate.exe add monparam oraclew oraclew relation/type grp
.\clpcreate.exe add monparam oraclew oraclew relation/name failover-oracle
.\clpcreate.exe add monparam oraclew oraclew agentparam/dbname sid
.\clpcreate.exe add monparam oraclew oraclew agentparam/password 80000006b17b582700630056   # password is "oracle"
if ($type -eq 0)
{
    .\clpcreate.exe add monparam oraclew oraclew emergency/threshold/restart 0
}
elseif ($type -eq 1)
{
    .\clpcreate.exe add monparam oraclew oraclew emergency/threshold/restart 0
    .\clpcreate.exe add monparam oraclew oraclew emergency/timeout/notreconfirmation/use 1
    .\clpcreate.exe add monparam oraclew oraclew emergency/timeout/notrecovery/use 1
}
elseif ($type -eq 2)
{
    .\clpcreate.exe add monparam oraclew oraclew relation/type cls
    .\clpcreate.exe add monparam oraclew oraclew relation/name LocalServer
    .\clpcreate.exe add monparam oraclew oraclew emergency/action 6
}


# add object number
$srvnum = $server.Length - 1
Write-Output "srvnum: $srvnum"
$hbnum = ($hb.Length - 1) + ($diskhb.Length - 1)
Write-Output "hbnum: $hbnum"
$npnum = ($pingnp.Length - 1)
Write-Output "npnum: $npnum"
$grpnum = $group.Length - 1
Write-Output "grpnum: $grpnum"
for ($i = 0; $i -lt ($resource.Length - 1); $i++) 
{
    for ($j = 0; $j -lt $resource[$i].Length; $j++) 
    {
        $rscnum++
    }
}
Write-Output "rscnum: $rscnum"
$monnum = $monitor.Length
Write-Output "monnum: $monnum"
$objnum = $srvnum + ($srvnum * $hbnum) + ($srvnum * $npnum) + $grpnum + $rscnum + $monnum + 4
Write-Output "objnum: $objnum"
.\clpcreate.exe add objnum $objnum

# set encode
.\clpcreate.exe add encode $cluster["encode"]
