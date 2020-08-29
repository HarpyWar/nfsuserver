
# Need For Speed: Underground Server

This server was initially developed by [3 PriedeZ](http://3priedez.net)

It doesn't support NFSU2, only first NFSU.

## Server Installation

### Linux

Download static binary from [Releases](https://github.com/HarpyWar/nfsuserver/releases) or build it from sources:

```
apt-get install git make build-essentials
git clone https://github.com/HarpyWar/nfsuserver.git
cd nfsuserver/nfsuserver
make
```

### Windows

Download `nfsuserver.1.0.3.win32.zip` from [Releases](https://github.com/HarpyWar/nfsuserver/releases) or use Visual Studio to compile own executable (uncomment [NT_SERVICE](https://github.com/HarpyWar/nfsuserver/blob/master/nfsuserver/win_nix.h#L5) flag for Windows service mode.).

`nfsuserver2.exe` is for normal start, not a service.

To install a server as a Windows Service run:
```
nfsuserver2_svc.exe -i
```
To uninstall run:
```
nfsuserver2_svc.exe -u
```




## Server Configuration

1. Create a new file `nfsu.ini` with the following contents:

```
[NFSU:LAN]
servername=Server_Name
enablelogfile=1
clearlogfile=0
enablelogscreen=1
rewritelogfile=0
registerglobal=0
disabletimestamp=0
verbose=0
logalltraffic=0
banv1=0
banv2=0
banv3=0
banv4=0
```

2. Create a new file `news` with a custom news contents.

3. After first start `server.log` should be created. After first user login a database `rusers.dat` with users should be created.


## Client Configuration

1. Install a game client [Need for Speed: Underground](https://en.wikipedia.org/wiki/Need_for_Speed:_Underground)

2. Download and unzip [nfsupatch14.zip](https://github.com/HarpyWar/nfsuserver/releases/download/client/nfsupatch14.zip) into your game folder, run `dev-rtp.exe` to upgrage the game client to version 1.4

3. *(Optional)* Download and unzip [speed14-nocd.zip](https://github.com/HarpyWar/nfsuserver/releases/download/client/speed14-nocd.zip) into your game folder with replacement of `Speed.exe` (you will be able to run the game without a CD)

4. Download and run [nfsuclient.exe](https://github.com/HarpyWar/nfsuserver/releases/download/client/nfsuclient.exe)

5. Click `Add Server`, put your server address, and then click `Use Server`

7. Run the game &rarr; Select `Play Online` &rarr; Create a new profile, agree with rules and click `Use Profile`

After a login you will join to a room called `Default`.

![NFSU Lobby](http://i.imgur.com/ntGM3VF.jpg)


## Documentation

* [Explaining Config](https://github.com/HarpyWar/nfsuserver/wiki/Explaining-Config)
* [Modify Channel Names](https://github.com/HarpyWar/nfsuserver/wiki/Modify-Channel-Names)
* [NFSU Server Status](https://github.com/HarpyWar/nfsuserver/wiki/NFSU-Server-Status)

