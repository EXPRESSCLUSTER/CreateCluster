##################################################
# set type for final action of monitor resources
# 0: use default final action (do nothing)
# 1: ignore timeout
# 2: initiate BSoD/panic immediately
$type = 1
##################################################

##################################################
# set parameters
$cluster = @{name = "cluster"; encode = "ASCII"; os = "linux"}
$server1 = @{name = "server1"} <# master node #>
$server2 = @{name = "server2"}
$ip = @(@{ip_srv1 = "192.168.137.71"; ip_srv2 = "192.168.137.72"; device_id = "0"},
        @{ip_srv1 = "192.168.138.71"; ip_srv2 = "192.168.138.72"; device_id = "1"},
        @{ip_srv1 = "192.168.139.71"; ip_srv2 = "192.168.139.72"; device_id = "2"},
        @{ip_srv1 = "192.168.140.71"; ip_srv2 = "192.168.140.72"; device_id = "3"})
$hb = @(@{device_id = "0"; priority = "0"},
        @{device_id = "1"; priority = "1"},
        @{device_id = "2"; priority = "2"},
        @{device_id = "3"; priority = "3"})
$diskhb = @(@{device = "/dev/sdc1"; priority = "4"})
$np = @(@{ip = "192.168.137.1"},
        @{ip = "192.168.137.100"})
$group = @(@{name = "failover"})
$resource = @(@{group = "failover"; type = "fip"; name = "fip1"; ip = "192.168.137.70"},
              @{group = "failover"; type = "disk"; name = "sd1"; disk_type = "lvm"; device = "/dev/ecxsd/sd1"; mount = "/mnt/sd1"; fs = "ext3"},
              @{group = "failover"; type = "volmgr"; name = "volmgr1"; volmgr_type = "lvm"; vg_name = "ecxsd"})
$resource_dependency = @(@{depending_resource = "sd1"; depended_resource = "volmgr1"})
$monitor = @(@{type = "userw"; name = "userw"},
             @{type = "fipw"; name = "fipw1"; monitor_fip = "fip1"; recovery_target_type = "grp"; recovery_target_name = "failover"},
             @{type = "volmgrw"; name = "volmgrw1"; monitor_timing = "volmgr1"; monitor_vg = "ecxsd"; recovery_target_type = "grp"; recovery_target_name = "failover"},
             @{type = "diskw"; name = "diskw1"; monitor_device = "/dev/sdc2"; recovery_target_type = "grp"; recovery_target_name = "failover"},
             @{type = "ipw"; name = "ipw1"; monitor_ip = "192.168.137.1"; recovery_target_type = "grp"; recovery_target_name = "failover"})
##################################################







##################################################
# Please do not edit the following content
##################################################

##################################################
# initialize
.\.\clpcreate.exe init


# add a cluster to initial configuration
.\.\clpcreate.exe add cls $cluster["name"] $cluster["encode"] $cluster["os"]

# add a server to a cluster
.\clpcreate.exe add srv $server1["name"] 0
.\clpcreate.exe add srv $server2["name"] 1

# add IP address to a server
for ($i = 0; $i -lt $ip.Length; $i++) {
    .\clpcreate.exe add ip $server1["name"] $ip[$i]["device_id"] $ip[$i]["ip_srv1"]
    .\clpcreate.exe add ip $server2["name"] $ip[$i]["device_id"] $ip[$i]["ip_srv2"]
}

# add a heartbeat interface to a cluster
for ($i = 0; $i -lt $hb.Length; $i++) {
    .\clpcreate.exe add hb $hb[$i]["device_id"] $hb[$i]["priority"]
}

# add a disk heartbeat interface to a cluster
for ($i = 0; $i -lt $diskhb.Length; $i++) {
.\clpcreate.exe add diskhb 300 $diskhb[$i]["priority"] $diskhb[$i]["device"] $server1["name"] $server2["name"]
}

# add a NP resource to a cluster
for ($i = 0; $i -lt $np.Length; $i++) {
    .\clpcreate.exe add np $i $np[$i]["ip"] $server1["name"] $server2["name"]
}

# add a group to a cluster
for ($i = 0; $i -lt $group.Length; $i++) {
    .\clpcreate.exe add grp failover $group[$i]["name"]
}

# add a resouce to a group
for ($i = 0; $i -lt $resource.Length; $i++) {
    if ($resource[$i]["type"] -eq "fip") {
        .\clpcreate.exe add rsc $resource[$i]["group"] $resource[$i]["type"] $resource[$i]["name"]
        .\clpcreate.exe add rscparam $resource[$i]["type"] $resource[$i]["name"] ip $resource[$i]["ip"]
    } elseif ($resource[$i]["type"] -eq "disk") {
        .\clpcreate.exe add rsc $resource[$i]["group"] $resource[$i]["type"] $resource[$i]["name"]
        .\clpcreate.exe add rscparam $resource[$i]["type"] $resource[$i]["name"] disktype $resource[$i]["disk_type"]
        .\clpcreate.exe add rscparam $resource[$i]["type"] $resource[$i]["name"] device $resource[$i]["device"]
        .\clpcreate.exe add rscparam $resource[$i]["type"] $resource[$i]["name"] mount/point $resource[$i]["mount"]
        .\clpcreate.exe add rscparam $resource[$i]["type"] $resource[$i]["name"] fs $resource[$i]["fs"]
    } elseif ($resource[$i]["type"] -eq "volmgr") {
        .\clpcreate.exe add rsc $resource[$i]["group"] $resource[$i]["type"] $resource[$i]["name"]
        .\clpcreate.exe add rscparam $resource[$i]["type"] $resource[$i]["name"] type $resource[$i]["volmgr_type"]
        .\clpcreate.exe add rscparam $resource[$i]["type"] $resource[$i]["name"] devname $resource[$i]["vg_name"]
    } else {
        Write-Host "The resource type " + $resource[$i]["type"] + " does not exist."
        exit 0
    }
}

# add dependencies of a resource
for ($i = 0; $i -lt $resource_dependency.Length; $i++) {
    $resource_type = "none"
    for ($j = 0; $j -lt $resource.Length; $j++) {
        if ($resource[$j]["name"] -eq $resource_dependency[$i]["depending_resource"]) {
            $resource_type = $resource[$j]["type"]
        }
    }
    if ($resource_type -eq "none") {
        Write-Host "The resource " + $resource_dependency[$i]["depending_resource"] + " does not exist."
        exit 0
    }
    .\clpcreate.exe add rscdep $resource_type $resource_dependency[$i]["depending_resource"] $resource_dependency[$i]["depended_resource"]
}

# add a monitor to a cluster
for ($i = 0; $i -lt $monitor.Length; $i++) {
    if ($monitor[$i]["type"] -eq "userw") {
        .\clpcreate.exe add mon $monitor[$i]["type"] $monitor[$i]["name"]
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] relation/type cls
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] relation/name LocalServer
        continue
    } elseif ($monitor[$i]["type"] -eq "fipw") {
        .\clpcreate.exe add mon $monitor[$i]["type"] $monitor[$i]["name"]
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] target $monitor[$i]["monitor_fip"]
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] parameters/monmii 1
    } elseif ($monitor[$i]["type"] -eq "volmgrw") {
        .\clpcreate.exe add mon $monitor[$i]["type"] $monitor[$i]["name"]
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] parameters/devname $monitor[$i]["monitor_vg"]
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] target $monitor[$i]["monitor_timing"]
    } elseif ($monitor[$i]["type"] -eq "diskw") {
        .\clpcreate.exe add mon $monitor[$i]["type"] $monitor[$i]["name"]
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] parameters/object $monitor[$i]["monitor_device"]
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] parameters/size 512
    } elseif ($monitor[$i]["type"] -eq "ipw") {
        .\clpcreate.exe add mon $monitor[$i]["type"] $monitor[$i]["name"]
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] parameters/list@0/ip $monitor[$i]["monitor_ip"]
    } else {
        Write-Host "The monitor type " + $monitor[$i]["type"] + " does not exist."
        exit 0
    }

    .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] relation/type $monitor[$i]["recovery_target_type"]
    .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] relation/name $monitor[$i]["recovery_target_name"]

    <# finalaction depending type #>
    if ($type -eq 0)
    {
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] emergency/threshold/restart 0
    }
    elseif ($type -eq 1)
    {
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] emergency/threshold/restart 0
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] emergency/timeout/notreconfirmation/use 1
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] emergency/timeout/notrecovery/use 1
    }
    elseif ($type -eq 2)
    {
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] relation/type cls
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] relation/name LocalServer
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] emergency/action 10
    }
}

# add object number
$obj = 6  <# 1 cluster + 1 Servers + 1 Groups + 1 Monitors + 2 servers #>
$obj = $obj + 2 * ($hb.Length + $diskhb.Length + 1) + $group.Length + $resource.Length + $monitor.Length
.\clpcreate.exe add objnum $obj