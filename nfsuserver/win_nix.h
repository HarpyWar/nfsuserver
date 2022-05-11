#ifndef _WIN_NIX_H_
#define _WIN_NIX_H_

//define NT_SERVICE to enable service mode
//#define NT_SERVICE

#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <vector>
#include <algorithm>

#ifdef _WIN32
#include <Ws2tcpip.h>
#ifdef NT_SERVICE
#include <windows.h>
#include <Winsvc.h>
#endif
#include <process.h>
#include <winsock.h>
#else
#undef NT_SERVICE
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#endif
#ifdef _WIN32
#define RETURNFROMTHREAD return
#else
#define RETURNFROMTHREAD return NULL
#define SOCKADDR sockaddr
#define SOCKADDR_IN sockaddr_in
#define _strdup strdup
#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define HANDLE pthread_t
#define BOOL bool
#define DWORD long
#define TRUE 1
#define FALSE 0
#define SOCKET_ERROR -1
#define Sleep(x) usleep(x*1000)
#define closesocket close
#define stricmp strcasecmp
#define _beginthread(func, stack, data) pthread_create(&glob_hack_tid, NULL, (void*(*)(void*))func, data)
static pthread_t glob_hack_tid;
#endif
#ifdef _WIN32
#define threadfunc void
#define SERVER_PLATFORM "win32"
#else
#define threadfunc void *
#define SERVER_PLATFORM "*nix"
#endif

#endif //_WIN_NIX_H_
