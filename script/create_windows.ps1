##################################################
# Set type for final action of monitor resources
# 0: use default final action (do nothing)
# 1: ignore timeout
# 2: initiate BSoD/panic immediately
$type = 1
##################################################

##################################################
# Set parameters

# encode:
#   Japanese -> SJIS, English -> ASCII
# os:
#   windows only
$cluster = @{name = "cluster"; encode = "ASCII"; os = "windows"}

# Server 1 is a master node
$server1 = @{name = "auto-vm1-2016"}
$server2 = @{name = "auto-vm2-2016"}

# ip_srv1, 2 is used to send a heartbeat eachother.
# ip_srv1 and ip_srv2 that have a same device_id are needed to be in a same network.
$ip = @(@{ip_srv1 = "192.168.137.10"; ip_srv2 = "192.168.137.20"; device_id = "0"},
        @{ip_srv1 = "192.168.138.10"; ip_srv2 = "192.168.138.20"; device_id = "1"},
        @{ip_srv1 = "192.168.139.10"; ip_srv2 = "192.168.139.20"; device_id = "2"},
        @{ip_srv1 = "192.168.140.10"; ip_srv2 = "192.168.140.20"; device_id = "3"})

# Heartbeat
$hb = @(@{device_id = "0"; priority = "0"},
        @{device_id = "1"; priority = "1"},
        @{device_id = "2"; priority = "2"},
        @{device_id = "3"; priority = "3"})

# Ping network partition
# If a server cannot reach all IP addresses, the server judges itself to be isolated from the network.
$pingnp = @(@{ip = "192.168.137.75"},
        @{ip = "192.168.137.100"})

# Disk network partition
# Only one disknp can be set. 
$disknp = @(@{drive_letter = "I"})

# Setting of hba filter
$hba = @(@{id = "0"; server = "auto-vm1-2016"; port = "3"; device_id = "ROOT\ISCSIPRT"; instance_id = "0000"},
         @{id = "0"; server = "auto-vm2-2016"; port = "3"; device_id = "ROOT\ISCSIPRT"; instance_id = "0000"})

# Failover group
$group = @(@{name = "failover"})

# Group resource
$resource = @(@{group = "failover"; type = "fip"; name = "fip1"; ip = "192.168.137.30"},
              @{group = "failover"; type = "sd"; name = "sd1"; drive_letter = "J"; guid = "47cf9225-880c-4925-8e13-6dd03a169cb4"},
              @{group = "failover"; type = "script"; name = "script1"})
$resource_dependency = @(@{depending_resource = "script1"; depended_resource = "sd1"})

# Monitor resource
$monitor = @(@{type = "userw"; name = "userw"},
             @{type = "fipw"; name = "fipw1"; monitor_fip = "fip1"; recovery_target_type = "grp"; recovery_target_name = "failover"},
             @{type = "ipw"; name = "ipw1"; monitor_ip = "192.168.137.1"; recovery_target_type = "grp"; recovery_target_name = "failover"},
             @{type = "sdw"; name = "sdw1"; monitor_sd = "sd1"; recovery_target_type = "grp"; recovery_target_name = "failover"},
             @{type = "genw"; name = "genw1"; recovery_target_type = "grp"; recovery_target_name = "failover"})
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

# add a ping NP resource to a cluster
$pingnp_priority = -1
if ($disknp.Length -eq 0) {
    $pingnp_priority = 0
} else {
    $pingnp_priority = 1
    for ($i = 0; $i -lt $disknp.Length; $i++) {
        $disknp[$i]["drive_letter"] = $disknp[$i]["drive_letter"] + ":\"
    }
}
for ($i = 0; $i -lt $pingnp.Length; $i++) {
    .\clpcreate.exe add pingnp $i $pingnp_priority $pingnp[$i]["ip"] $server1["name"] $server2["name"]
}

# add a disk NP resource to a cluster
for ($i = 0; $i -lt $disknp.Length; $i++) {
    .\clpcreate.exe add disknp $i 0 $disknp[$i]["drive_letter"] $server1["name"] $server2["name"]
}

# set HBA filters
for ($i = 0; $i -lt $hba.Length; $i++) {
    .\clpcreate.exe add hba $hba[$i]["server"] $hba[$i]["id"] portnumber $hba[$i]["port"]
    .\clpcreate.exe add hba $hba[$i]["server"] $hba[$i]["id"] deviceid $hba[$i]["device_id"]
    .\clpcreate.exe add hba $hba[$i]["server"] $hba[$i]["id"] instanceid $hba[$i]["instance_id"]
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
    } elseif ($resource[$i]["type"] -eq "sd") {
        .\clpcreate.exe add rsc $resource[$i]["group"] $resource[$i]["type"] $resource[$i]["name"]
        .\clpcreate.exe add rscparam $resource[$i]["type"] $resource[$i]["name"] volumemountpoint $resource[$i]["drive_letter"]
        .\clpcreate.exe add rscguid $resource[$i]["type"] $resource[$i]["name"] $server1["name"] volumeguid $resource[$i]["guid"]
        .\clpcreate.exe add rscguid $resource[$i]["type"] $resource[$i]["name"] $server2["name"] volumeguid $resource[$i]["guid"]
    } elseif ($resource[$i]["type"] -eq "script") {
        .\clpcreate.exe add rsc $resource[$i]["group"] $resource[$i]["type"] $resource[$i]["name"]
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
    } elseif ($monitor[$i]["type"] -eq "ipw") {
        .\clpcreate.exe add mon $monitor[$i]["type"] $monitor[$i]["name"]
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] parameters/list@0/ip $monitor[$i]["monitor_ip"]
    } elseif ($monitor[$i]["type"] -eq "sdw") {
        .\clpcreate.exe add mon $monitor[$i]["type"] $monitor[$i]["name"]
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] parameters/object $monitor[$i]["monitor_sd"]
    } elseif ($monitor[$i]["type"] -eq "genw") {
        .\clpcreate.exe add mon $monitor[$i]["type"] $monitor[$i]["name"]
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
        .\clpcreate.exe add monparam $monitor[$i]["type"] $monitor[$i]["name"] emergency/action 6
    }
}

# add object number
$obj = 6  <# 1 cluster + 1 Servers + 1 Groups + 1 Monitors + 2 servers #>
$pingnp_num = -1
$disknp_num = -1
if ($pingnp.Length -eq 0) {
    $pingnp_num = 0
} else {
    $pingnp_num = 1
}
if ($disknp.Length -eq 0) {
    $disknp_num = 0
} else {
    $disknp_num = 1
}
$obj = $obj + 2 * ($hb.Length + $pingnp_num + $disknp_num) + $group.Length + $resource.Length + $monitor.Length
.\clpcreate.exe add objnum $obj

# set encode
.\clpcreate.exe add encode $cluster["encode"]

# convert CRLF to LF
$filepath = ".\clp.conf"
[text.encoding]::getencoding($cluster["encode"]).getbytes((get-content $filepath -encoding string) -join "`n")|set-content -encoding byte $filepath