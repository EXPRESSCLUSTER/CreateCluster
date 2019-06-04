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
$cluster = @{name = "mycluster"; encode = "SJIS"; os = "windows"}
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
$disknp = @(@("disknp1", "0", "10100"),
            @())
$disknpsrv = @(@("10100", "019056a3-a7ad-4de3-9ed8-d5e752e501ea", "R:\"),
               @(("10100", "019056a3-a7ad-4de3-9ed8-d5e752e501ea", "R:\")),
               @())
#
# $pingnp
#
$pingnp = @(@("pingnp1", "1", "10200", "0", "0", "192.168.1.144"),
            @())
#
# $group
#
$group = @(@("failover-oracle"),
           @())
#
# $resource
#
$resource = @(@(@("fip","fip", @("ip", "192.168.1.199")), 
                @("sd", "sd", @("volumemountpoint", "S:")),
                @("service", "service-db", @("name", "OracleServiceSID")),
                @("service", "service-listener", @("name", "OracleOraDB12Home1TNSListener")),
                @("script", "script-db")),
              @())
#
# $rscdepend
#
$rscdepend = @(@("fip", "sd"),
               @("sd", "service-db"),
               @("service-db", "service-listener"),
               @("service-listener", "script-db"),
               @())
#
# $monitor
$monitor = @(@("userw", "userw"),
             @("ipw", "ipw-gw", @("relation/type", "grp"), @("relation/name", $group[0]), @("parameters/list@1/ip", "192.168.1.144")),
             @("fipw", "fipw", @("parameters/monmii", "1"), @("target", "fip"), @("relation/type", "grp"), @("relation/name", $group[0])),
             @("servicew", "servicew-db", @("target", "service-db"), @("relation/type", "grp"), @("relation/name", $group[0])),
             @("servicew", "servicew-listener", @("target", "service-listener"), @("relation/type", "grp"), @("relation/name", $group[0])),
             @("oraclew", "oraclew", @("target", "script-db"), @("relation/type", "grp"), @("relation/name", $group[0]), @("agentparam/dbname", "sid"), @("agentparam/password", "80000006b17b582700630056")))
#
# $hba
$hba = @(@($server[0], "0", "3", "ROOT\ISCSIPRT", "0000"),
         @($server[1], "0", "3", "ROOT\ISCSIPRT", "0000"))
#
# $rscguid
$rscguid = @(@($server[0], "sd", "205bce9a-e322-442d-af11-776f8b6af913"),
             @($server[1], "sd", "205bce9a-e322-442d-af11-776f8b6af913"),
             @())
##################################################


##################################################
# You don't need to change the following lines, maybe.
#
# initialize
.\clpcreate.exe init

# add a cluster to initial configuration
.\clpcreate.exe add cls $cluster["name"] $cluster["encode"] $cluster["os"]

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
for ($i = 0; $i -lt $hba.Length; $i++)
{
    .\clpcreate.exe add hba $hba[$i][0] $hba[$i][1] portnumber $hba[$i][2]
    .\clpcreate.exe add hba $hba[$i][0] $hba[$i][1] deviceid $hba[$i][3]
    .\clpcreate.exe add hba $hba[$i][0] $hba[$i][1] deviceid $hba[$i][4]
}

# add a network partition resource to a cluster
for ($i = 0; $i -lt ($pingnp.Length - 1); $i++) 
{
    .\clpcreate.exe add np disk $disknp[$i][0] $disknp[$i][1] $disknp[$i][2]
}
for ($i = 0; $i -lt ($pingnp.Length - 1); $i++) 
{
    .\clpcreate.exe add np ping $pingnp[$i][0] $pingnp[$i][1] $pingnp[$i][2] $pingnp[$i][3] $pingnp[$i][4] $pingnp[$i][5]
}

# add a network partition to a server
for ($i = 0; $i -lt ($server.Length - 1); $i++) 
{
    .\clpcreate.exe add npsrv disk $server[$i] $disknpsrv[$i][0] $disknpsrv[$i][1] $disknpsrv[$i][2]
}
for ($i = 0; $i -lt ($server.Length - 1); $i++) 
{
    for ($j = 0; $j -lt ($pingnp.Length - 1); $j++)
    {
        .\clpcreate.exe add npsrv ping $server[$i] $pingnp[$j][2] $pingnp[$j][1] 
    }
}

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

# add GUID to a resource
for ($i = 0; $i -lt ($rscguid.Length - 1); $i++) 
{
    for ($j = 0; $j -lt ($resource.Length - 1); $j++) 
    {
        for ($k = 0; $k -lt $resource[$j].Length; $k++)
        {
            if ($rscguid[$i][1] -eq $resource[$j][$k][1])
            {
                .\clpcreate.exe add rscguid $resource[$j][$k][0] $resource[$j][$k][1] $rscguid[$i][0] volumeguid $rscguid[$i][2]
            }
        }
    }
}
# add a monitor resource to a cluster
for ($i = 0; $i -lt $monitor.Length; $i++) 
{
    .\clpcreate.exe add mon $monitor[$i][0] $monitor[$i][1]
    for ($j = 2; $j -lt $monitor[$i].Length; $j++) 
    {
        .\clpcreate.exe add monparam $monitor[$i][0] $monitor[$i][1] $monitor[$i][$j][0] $monitor[$i][$j][1]
    }
    if ($monitor[$i][0] -eq "userw")
    {
        .\clpcreate.exe add monparam $monitor[$i][0] $monitor[$i][1] relation/type cls
        .\clpcreate.exe add monparam $monitor[$i][0] $monitor[$i][1] relation/name LocalServer
    }
    else 
    {
        if ($type -eq 0)
        {
            <# do nothing #>>
        }
        elseif ($type -eq 1)
        {
            .\clpcreate.exe add monparam $monitor[$i][0] $monitor[$i][1] emergency/threshold/restart 0
            .\clpcreate.exe add monparam $monitor[$i][0] $monitor[$i][1] emergency/timeout/notreconfirmation/use 1
            .\clpcreate.exe add monparam $monitor[$i][0] $monitor[$i][1] emergency/timeout/notrecovery/use 1
        }
        elseif ($type -eq 2)
        {
            # set BSoD (6)
            .\clpcreate.exe add monparam $monitor[$i][0] $monitor[$i][1] relation/type cls
            .\clpcreate.exe add monparam $monitor[$i][0] $monitor[$i][1] relation/name LocalServer
            .\clpcreate.exe add monparam $monitor[$i][0] $monitor[$i][1] emergency/action 6
        }
    }
}

# add object number
$srvnum = $server.Length - 1
Write-Output "srvnum: $srvnum"
$hbnum = ($hb.Length - 1)
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
