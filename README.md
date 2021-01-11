
# Need For Speed: Underground Server

This server was initially developed by [3 PriedeZ](http://3priedez.net)

It support only 1-st [Need for Speed: Underground](https://en.wikipedia.org/wiki/Need_for_Speed:_Underground), but not Underground 2.

## Server Installation

### Linux

Download static binary from [Releases](https://github.com/HarpyWar/nfsuserver/releases) or build it from sources:

```
apt-get install git make build-essential
git clone https://github.com/HarpyWar/nfsuserver.git
cd nfsuserver/nfsuserver
make
./nfsuserver
```

### Windows

Download `nfsuserver.1.0.4.win32.zip` from [Releases](https://github.com/HarpyWar/nfsuserver/releases) or use Visual Studio to build own executable (uncomment [NT_SERVICE](https://github.com/HarpyWar/nfsuserver/blob/master/nfsuserver/win_nix.h#L5) flag for Windows service mode.). For compilation you have to install [Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk/).

`nfsuserver.exe` is for normal start, not a service.

To install a server as a Windows Service run:
```
nfsuserver_svc.exe -i
```
To uninstall run:
```
nfsuserver_svc.exe -u
```




## Server Configuration

Edit `nfsu.conf` and `news.txt` to suit your needs.

After first start `server.log` should be created. After first user login a database `rusers.dat` with users should be created.


## Client Configuration

1. Download and run [nfsuclient.exe](https://github.com/HarpyWar/nfsuserver/releases/download/client/nfsuclient.exe). Click on `Get Game` button to download ready to play NFSU game v1.4 with noCD and Widescreen with HD resolution.

2. Click `Get public serverlist` or `Add Server` to put your server address manually, and then click `Use Server`

3. Run the game &rarr; Select `Play Online` &rarr; Create a new profile, agree with rules and click `Use Profile`

After a login you have to select car and join a room to create/join a game.

![NFSU Lobby](http://i.imgur.com/ntGM3VF.jpg)


## Documentation

* [NFSU Server Status](https://github.com/HarpyWar/nfsuserver/wiki/NFSU-Server-Status)

