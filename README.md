# CreateCluster
## Notice
- From CLUSTERPRO X 5.1, we officially provide clpcfadm command to create a cluster configuration file and we have the reference guide for clpcfadm. Please use clpcfadm and the guide instead of this repository.
  - [CLUSTERPRO X](https://docs.nec.co.jp/sites/default/files/minisite/static/da9ec854-8953-42aa-acbe-fbf0deade99c/clp_x51_windows/W51_CFADM_JP/index.html)

## Purpose
- The purpose of this project is to provide command or DLL can run on **Windows** to create a cluster without GUI. If you want a command can run on **Linux**, please visit [CreateClusterOnLinux](https://github.com/EXPRESSCLUSTER/CreateClusterOnLinux).

### clpcreate
- This is a command (clpcreate.exe) to create a cluster. For command line, please refer to [CheateSheet](https://github.com/EXPRESSCLUSTER/CreateCluster/blob/develop/CheatSheet.md). And we have some sample scripts on [script](https://github.com/EXPRESSCLUSTER/CreateCluster/tree/master/script) directory.

### clpconf
- This is a DLL (clpconf.dll) to provide APIs to create a cluster. For details of APIs, please refer to [clpconf.h](https://github.com/EXPRESSCLUSTER/CreateCluster/blob/master/clpconf/src/clpconf.h).
- There some sample code in sample-*** directories.

## Special Thanks
- Katsutoshi Hiroshige
- Noriki Morikata
- Tohru Hashimoto
- Shinzo Yamada
- Futoshi Takeda
- Koji Yasui
- [Taira Takemoto](https://github.com/tairametal)
- [Kazuyuki Miyamoto](https://github.com/mkazuyuki)
