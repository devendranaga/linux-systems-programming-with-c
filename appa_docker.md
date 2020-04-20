# Appendix. A docker


docker is an alternative but very different to the virtualisation platforms such as VirtualBox and VMWare.

starting docker daemon:
---------

`docker -d` as a root to start the deamon

docker images to list available docker images.

```bash
dev@hanzo:~$ docker images
REPOSITORY                   TAG                 IMAGE ID            CREATED             VIRTUAL SIZE
ubuntu                       latest              ab035c88d533        2 weeks ago         188 MB
xdrum/openwrt                latest              2ff262d9c211        5 months ago        5.2 MB
telsaleh/iot-discovery       latest              9403bdcb4a51        6 months ago        899.5 MB
openwrt-x86-generic-rootfs   latest              6558146e8176        10 months ago       5.283 MB
ubuntu                       14.04               07f8e8c5e660        11 months ago       188.3 MB
hello-world                  latest              91c95931e552        11 months ago       910 B
solarkennedy/openwrt         latest              6c74e6ce9d45        23 months ago       14.44 MB
```

docker ps to list containers.

```bash
dev@hanzo:~$ docker ps
CONTAINER ID        IMAGE               COMMAND             CREATED             STATUS              PORTS               NAMES
496911dda486        ubuntu:latest       "/bin/bash"         21 seconds ago      Up 20 seconds                           sharp_lumiere
```

docker logs <containerid> will gives us the latest screenshots of the commands that we performed.

```bash
dev@hanzo:~$ docker logs 496911dda486
root@496911dda486:/# ls
bin  boot  dev  etc  home  lib  lib64  media  mnt  opt  proc  root  run  sbin  srv  sys  tmp  usr  var
root@496911dda486:/# ps -e
  PID TTY          TIME CMD
    1 ?        00:00:00 bash
   21 ?        00:00:00 ps
root@496911dda486:/# ps -e
  PID TTY          TIME CMD
    1 ?        00:00:00 bash
   22 ?        00:00:00 ps
root@496911dda486:/# clear
```

docker run command makes a container run.

```bash
dev@hanzo:~$ docker run -a stdin -a stdout -i -t ubuntu /bin/bash
root@496911dda486:/#
```

when `docker run` is performed, it gives a  random name to the container. This can be avoided by providing a name at run time like below.

```bash
docker run --name my-ubuntu ubuntu:14.04
```

in the above command the name my-ubuntu will be the name of the running container.

docker info lists system wide information

```bash
dev@hanzo:~$ docker info
Containers: 31
Images: 49
Storage Driver: aufs
 Root Dir: /var/lib/docker/aufs
 Backing Filesystem: extfs
 Dirs: 111
 Dirperm1 Supported: true
Execution Driver: native-0.2
Kernel Version: 3.19.0-15-generic
Operating System: Ubuntu 15.04
CPUs: 1
Total Memory: 707.1 MiB
Name: hanzo
ID: T3XK:VG72:W6OE:W7E6:BZFE:YPZE:ZJLC:WPZL:WBGU:WRTY:RSU2:OWVU
Username: devnaga4
Registry: [https://index.docker.io/v1/]
```


docker stats display a live stream of one or more containers.

```bash
CONTAINER           CPU %               MEM USAGE/LIMIT       MEM %               NET I/O
496911dda486        0.00%               34.39 MiB/707.1 MiB   4.86%               21.58 MiB/492 KiB
```

stopping a container is done with the following command.

```bash
docker stop my-ubuntu
```
