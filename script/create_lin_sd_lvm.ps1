##################################################
# Sample script for Linux
##################################################
# Parameters
#-------------------------------------------------
# clpcreate.exe
#  Set the relative path for clpcreate.exe.
#clpcreate.exe = "clpcreate.exe"
#
# $type
#  Set type for final action of monitor resources.
#  0: use default final action (do nothing)
#  1: ignore timeout
#  2: initiate BSoD/panic immediately
$type = 0
#
# $cluster
#  encode:
#   Japanese: EUC-JP (for Linux)
#   English : ASCII
#   Chinese : GB2312
#  os:
#   windows or linux
$cluster = @{name = "cluster"; encode = "EUC-JP"; os = "linux"}
#
# $server
#  Top of the list is master node.
$server = @(@("server1"),
            @("server2"),
            @())
# 
# $ip
#  Set IP addresses for the servers.
#  On this case, upper IP addresses for server1 and bottom IP addresses for server2.
$ip = @(@("192.168.137.71", "192.168.138.71", "192.168.139.71", "192.168.140.71"),
        @("192.168.137.72", "192.168.138.72", "192.168.139.72", "192.168.140.72"),
        @())
#
# $hb
#  Left : 
#  Right: 
$hb = @(@("0", "0"),
        @("1", "1"),
        @("2", "2"),
        @("3", "3"),
        @())
# 
# $diskhb
$diskhb = @(@("300", "4", "/dev/sdc1"),
            @())
#
# $pingnp
#
$pingnp = @(@("pingnp1", "0", "10200", "0", "0", "192.168.137.1"),
            @())
#
# $group
#
$group = @(@("failover1"),
           @())
#
# Group resource
$resource = @(@(@("fip","fip1", @("ip", "192.168.137.70")), 
                @("volmgr", "volmgr1", @("type", "lvm"), @("devname", "ecxsd")),
                @("disk", "disk1", @("disktype", "lvm"), @("device", "/dev/ecxsd/sd1"), @("mount/point", "/mnt/sd1"), @("fs", "ext3")),
                @("exec", "exec1")),
              @())
#
$rscdepend = @(@("volmgr1", "disk1"),
               @())
#
# $monitor
#  
$monitor = @(@("userw", "userw", @("parameters/method", "keepalive")),
             @("fipw", "fipw1", @("parameters/monmii", "1"), @("target", "fip"), @("relation/type", "grp"), @("relation/name", $group[0])),
             @("volmgrw", "volmgrw1", @("parameters/devname", "ecxsd"), @("target", "volmgr1"), @("relation/type", "grp"), @("relation/name", $group[0])),
             @("diskw", "diskw1", @("parameters/object", "/dev/sdc2"), @("relation/type", "grp"), @("relation/name", $group[0])),
             @("ipw", "ipw1", @("parameters/list@0/ip", "192.168.137.1"), @("relation/type", "grp"), @("relation/name", $group[0])),
             @("genw", "genw1", @("relation/type", "grp"), @("relation/name", $group[0]))
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

# add a disk heartbeat to a cluster
for ($i = 0; $i -lt ($diskhb.Length - 1); $i++) 
{
    .\clpcreate.exe add diskhb $diskhb[$i][0] $diskhb[$i][1] 
}

# add a disk heartbeat to a server
for ($i = 0; $i -lt ($server.Length - 1); $i++) 
{
    for ($j = 0; $j -lt ($diskhb.Length - 1); $j++) 
    {
        .\clpcreate.exe add diskhbsrv $server[$i] $diskhb[$j][0] $diskhb[$j][2] 
    }
}    

# add a network partition resource to a cluster
for ($i = 0; $i -lt ($pingnp.Length - 1); $i++) 
{
    .\clpcreate.exe add np ping $pingnp[$i][0] $pingnp[$i][1] $pingnp[$i][2] $pingnp[$i][3] $pingnp[$i][4] $pingnp[$i][5]
}

# add a network partition resource to a server
for ($i = 0; $i -lt ($server.Length - 1); $i++) 
{
    for ($j = 0; $j -lt ($pingnp.Length - 1); $j++)
    {
        .\clpcreate.exe add npsrv ping $server[$i] $pingnp[$j][2] 1
    }
}

# add a failover group to a cluster
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

# add a monitor resource to a cluster
for ($i = 0; $i -lt ($monitor.Length - 1); $i++) 
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
            # set keepalive panic (10)
            .\clpcreate.exe add monparam $monitor[$i][0] $monitor[$i][1] relation/type cls
            .\clpcreate.exe add monparam $monitor[$i][0] $monitor[$i][1] relation/name LocalServer
            .\clpcreate.exe add monparam $monitor[$i][0] $monitor[$i][1] emergency/action 10
        }
    }
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

# convert CRLF to LF
$filepath = ".\clp.conf"
[text.encoding]::getencoding($cluster["encode"]).getbytes((get-content $filepath -encoding string) -join "`n")|set-content -encoding byte $filepath
