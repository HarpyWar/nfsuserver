#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Urlmon.lib")

#include <winsock2.h>
#include <windows.h>
#include <commctrl.h>
#include <process.h>
//#include <Iphlpapi.h>
//#include <commctrl.h>
#include <stdio.h>
#include <malloc.h>
#include "resource.h"
#include "servers.h"
#include "..\nfsuserver\objects.h"
#include <Tlhelp32.h>

ClServersClass Servers;

HWND dwnd;
HINSTANCE hinst;
HWND hList;
bool gotnolan = true;
SOCKET listenerSocket;
SOCKET RedirectSocket;
ConnectionsClass RedirectConnections;
unsigned int ids = 0;
time_t curtime;

#define INI_FILE "nfsulan.ini"

char fname[1024];
int curitem;
ClServerClass * CurrentServer;

void RunSpeed() {
	//HKEY_LOCAL_MACHINE\SOFTWARE\EA GAMES\Need For Speed Underground
	HKEY hKey;

	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, "SOFTWARE\\EA GAMES\\Need For Speed Underground", 0, KEY_QUERY_VALUE,
					  &hKey) == ERROR_SUCCESS) {
		char buf[1024];
		DWORD len = 1024;

		if (RegQueryValueEx (hKey, "Install Dir", NULL, NULL, (LPBYTE)buf, &len) == ERROR_SUCCESS) {
			//strcat(buf, "speed.exe");
			if ((int)ShellExecute (0, "open", "Speed.exe", NULL, buf, SW_SHOW) < 33) {
				sprintf (buf, "ShellExecute error: %u", GetLastError ());
				MessageBox (0, buf, "Error", MB_OK);
			}
		}
		RegCloseKey (hKey);
	}
} ;

char *strstri( char *s1, char *s2 ) {
	int i, j, k;

	for (i = 0; s1[i]; i++)
		for (j = i, k = 0; tolower (s1[j]) == tolower (s2[k]); j++, k++)
			if (!s2[k + 1]) return (s1 + i);

	return NULL;
}

bool IsSpeedOnline() {
	PROCESSENTRY32 proc;
	proc.dwSize = sizeof (PROCESSENTRY32);
	HANDLE snapshot;

	snapshot = CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);

	Process32First (snapshot, &proc);

	if (strstri (proc.szExeFile, "speed.exe")) {
		CloseHandle (snapshot);
		return true;
	}

	while (TRUE == Process32Next (snapshot, &proc))
		if (strstri (proc.szExeFile, "speed.exe")) {
			CloseHandle (snapshot);
			return true;
		}

	CloseHandle (snapshot);
	return false;
} ;

void ClearHosts() {
	FILE * fil;
	char buf[1024];
	char * out;
	fil = fopen (fname, "r");

	if (fil != NULL) {
		fseek (fil, 0, SEEK_END);
		int fsiz = ftell (fil);
		fseek (fil, 0, SEEK_SET);
		out = (char *)calloc (fsiz + 1024, sizeof (char));

		while (!feof (fil)) {
			if (fgets (buf, 1024, fil) != NULL) {
				if (strstr (buf, "ps2nfs04.ea.com") == NULL) {
					strcat (out, buf);
				}
			}
		}

		fclose (fil);
		fil = fopen (fname, "w");

		if (fil != NULL) {
			fwrite (out, 1, strlen (out), fil);
			fclose (fil);
		} else {
			char buf[1000];
			sprintf (buf, "Could not write hosts file. Make sure that %s is writable", fname);
			MessageBox (0, buf, "Error", MB_OK);
		}
		free (out);
	}
} ;

bool WriteHosts() {
	CurrentServer->Update ();

	if (!CurrentServer->IsOnline) return false;

	if (CurrentServer->ip == -1) return false;

	FILE * fil;
	char buf[1024];
	char * out;
	fil = fopen (fname, "r");

	if (fil != NULL) {
		fseek (fil, 0, SEEK_END);
		int fsiz = ftell (fil);
		fseek (fil, 0, SEEK_SET);
		out = (char *)calloc (fsiz + 1024, sizeof (char));

		while (!feof (fil)) {
			if (fgets (buf, 1024, fil) != NULL) {
				if (strstr (buf, "ps2nfs04.ea.com") == NULL) {
					strcat (out, buf);
				}
			}
		}
		fclose (fil);
	} else {
		out = (char *)calloc (1024, sizeof (char));
	}

	char tm[1024];
	memset (tm, 0, 1024);
	GetDlgItemText (dwnd, IDC_IP, tm, 1024);

	if(RedirectSocket==INVALID_SOCKET){		
		struct in_addr addr;
		addr.S_un.S_addr = CurrentServer->ip;
		strcpy (tm, inet_ntoa (addr));
		sprintf(buf, "%s\tps2nfs04.ea.com", tm);
	}else{
		strcpy (buf, "127.0.0.1\tps2nfs04.ea.com");
	}
	strcat (out, buf);
	fil = fopen (fname, "w");

	if (fil != NULL) {
		fwrite (out, 1, strlen (out), fil);
		fclose (fil);
		free (out);

		//to be sure that dns will catch changes
		Sleep(1000);

		//checking if hosts file is working
		struct hostent *hostInfo = gethostbyname ("ps2nfs04.ea.com");		
		SOCKADDR_IN remote_sockaddr_in;
		if (hostInfo != NULL) {
			memcpy(&remote_sockaddr_in.sin_addr.S_un.S_addr, hostInfo->h_addr_list[0], hostInfo->h_length);

			if(RedirectSocket==INVALID_SOCKET){
				if(strcmp(tm, inet_ntoa(remote_sockaddr_in.sin_addr))==0){
					return true;
				}else{
					MessageBox(dwnd, "ps2nfs04.ea.com didnt resolved as expected. Check hosts file.", "Error", MB_OK);
					return false;
				}
			}else{
				if(strcmp("127.0.0.1", inet_ntoa(remote_sockaddr_in.sin_addr))==0){
					return true;
				}else{
					MessageBox(dwnd, "ps2nfs04.ea.com didnt resolved as expected. Check hosts file.", "Error", MB_OK);
					return false;
				}
			}
			return true;
		} else {
			MessageBox(dwnd, "Could not resolve ps2nfs04.ea.com. Check hosts file.", "Error", MB_OK);
			return false;
		}
		return true;
	} else {
		char buf[1000];
		sprintf (buf, "Could not write hosts file. Make sure that %s is writable", fname);
		MessageBox (0, buf, "Error", MB_OK);
		free (out);
		return false;
	}
} ;

bool SendData( char *text ) {
	SOCKADDR_IN remote_sockaddr_in;
	remote_sockaddr_in.sin_family = AF_INET;
	remote_sockaddr_in.sin_port = htons (10800);
	remote_sockaddr_in.sin_addr.s_addr = inet_addr ("255.255.255.255");

	if (sendto (listenerSocket, text, strlen (text), (int)NULL, (SOCKADDR *)&remote_sockaddr_in,
				sizeof (SOCKADDR_IN)) == SOCKET_ERROR) {
		char erdes[1024];
		sprintf (erdes, "Error sending to socket: %i", WSAGetLastError ());
		return false;
	}

	return true;
}

bool SendData2( char *text, char *addr ) {
	struct hostent * hostInfo = gethostbyname (addr);
	SOCKADDR_IN remote_sockaddr_in;

	remote_sockaddr_in.sin_family = AF_INET;
	remote_sockaddr_in.sin_port = htons (10800);

	if (hostInfo != NULL) {
		memcpy (&remote_sockaddr_in.sin_addr.S_un.S_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
	} else {
		remote_sockaddr_in.sin_addr.s_addr = inet_addr (addr);
		if (remote_sockaddr_in.sin_addr.s_addr == INADDR_NONE) return false;
	}

	if (sendto (listenerSocket, text, strlen (text), (int)NULL, (SOCKADDR *)&remote_sockaddr_in,
				sizeof (SOCKADDR_IN)) == SOCKET_ERROR) {
		char erdes[1024];
		sprintf (erdes, "Error sending to socket: %i", WSAGetLastError ());
		return false;
	}

	return true;
}

void ListenerThread( void *dummy ) {
	int status;
	ClServerClass * tmp;
	SOCKADDR_IN remote_sockaddr_in;
	unsigned long remote_sockaddr_length = sizeof (SOCKADDR_IN);
	char tempBuff[1024];

	while (true) {
		status = recvfrom (listenerSocket, tempBuff, 1024, (int)NULL, (SOCKADDR *)&remote_sockaddr_in,
						   (int *)&remote_sockaddr_length);
		if (status == SOCKET_ERROR) {
			switch (WSAGetLastError ()) {
				case 10054:
					//ignore
					break;
				case WSAEINVAL:
					MessageBox (0, "Invalid args to recvfrom", "Error", MB_OK);
					closesocket (listenerSocket);
					return;
				default:
					sprintf (tempBuff, "Socket error: %u", WSAGetLastError ());
					MessageBox (0, tempBuff, "Error", MB_OK);
			}
		} else {
			tmp = Servers.ServerFromIP (remote_sockaddr_in.sin_addr.S_un.S_addr);
			if (tmp == NULL) {
				tmp = (ClServerClass *)calloc (1, sizeof (ClServerClass));
				strcpy (tmp->IP, inet_ntoa (remote_sockaddr_in.sin_addr));
				tmp->ip = remote_sockaddr_in.sin_addr.S_un.S_addr;
				Servers.AddServer (tmp);
				tmp->Update ();
			}
		}
	}

	closesocket (listenerSocket);
} ;

BOOL CALLBACK DlgAdd( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	ClServerClass * temp;

	switch (uMsg) {
		case WM_CLOSE:
			EndDialog (hwndDlg, 0);
			break;
		case WM_COMMAND:
			switch (LOWORD (wParam)) {
				case IDCANCEL:
					EndDialog (hwndDlg, 0);
					break;
				case IDC_OK:
					temp = (ClServerClass *)calloc (1, sizeof (ClServerClass));
					GetDlgItemText (hwndDlg, IDC_ADDR, temp->IP, 100);
					temp->IsFav = true;
					Servers.AddServer (temp);
					Servers.Save ();
					temp->Update ();
					EndDialog (hwndDlg, 0);
					break;
			}
			break;
		case WM_INITDIALOG:
			SetFocus (GetDlgItem (hwndDlg, IDC_ADDR));
			break;
	}

	return FALSE;
}

void upd( void *dummy ) {
	EnableWindow (GetDlgItem (dwnd, IDC_SERVERLIST), false);
	EnableWindow (GetDlgItem (dwnd, IDC_DELETE), false);
	EnableWindow (GetDlgItem (dwnd, IDC_ADD), false);
	EnableWindow (GetDlgItem (dwnd, IDC_FIND), false);
	EnableWindow (GetDlgItem (dwnd, IDC_REFRESH), false);
	EnableWindow (GetDlgItem (dwnd, IDC_GETLIST), false);
	EnableWindow (GetDlgItem (dwnd, IDC_LAN), false);
	Servers.Update ();
	EnableWindow (GetDlgItem (dwnd, IDC_SERVERLIST), true);
	EnableWindow (GetDlgItem (dwnd, IDC_DELETE), true);
	EnableWindow (GetDlgItem (dwnd, IDC_ADD), true);
	EnableWindow (GetDlgItem (dwnd, IDC_FIND), true);
	EnableWindow (GetDlgItem (dwnd, IDC_REFRESH), true);
	EnableWindow (GetDlgItem (dwnd, IDC_GETLIST), true);
	EnableWindow (GetDlgItem (dwnd, IDC_LAN), (CurrentServer != NULL));
} ;

void updw( void *dummy ) {
	EnableWindow (GetDlgItem (dwnd, IDC_LAN), false);
	EnableWindow (GetDlgItem (dwnd, IDC_SERVERLIST), false);
	EnableWindow (GetDlgItem (dwnd, IDC_DELETE), false);
	EnableWindow (GetDlgItem (dwnd, IDC_ADD), false);
	EnableWindow (GetDlgItem (dwnd, IDC_FIND), false);
	EnableWindow (GetDlgItem (dwnd, IDC_REFRESH), false);
	EnableWindow (GetDlgItem (dwnd, IDC_GETLIST), false);
	Servers.UpdateFromWeb ();
	Servers.Save ();
	EnableWindow (GetDlgItem (dwnd, IDC_LAN), (CurrentServer != NULL));
	EnableWindow (GetDlgItem (dwnd, IDC_SERVERLIST), true);
	EnableWindow (GetDlgItem (dwnd, IDC_DELETE), true);
	EnableWindow (GetDlgItem (dwnd, IDC_ADD), true);
	EnableWindow (GetDlgItem (dwnd, IDC_FIND), true);
	EnableWindow (GetDlgItem (dwnd, IDC_REFRESH), true);
	EnableWindow (GetDlgItem (dwnd, IDC_GETLIST), true);
} ;

BOOL CALLBACK MainDlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	LPNMHDR pnf;
	LV_ITEM it;
	LPNMITEMACTIVATE lit;

	switch (uMsg) {
		case WM_CLOSE:
			ClearHosts ();
			PostQuitMessage (0);
			break;
		case WM_NOTIFY:
			switch (wParam) {
				case IDC_SERVERLIST:
					pnf = (LPNMHDR)lParam;
					if (pnf->code == NM_CLICK) {
						lit = (LPNMITEMACTIVATE)lParam;
						it.iItem = lit->iItem;
						it.iSubItem = 0;
						it.mask = LVIF_PARAM;
						ListView_GetItem (hList, &it);
						EnableWindow (GetDlgItem (hwndDlg, IDC_DELETE), (lit->iItem != -1));
						EnableWindow (GetDlgItem (hwndDlg, IDC_LAN), (lit->iItem != -1));
						curitem = lit->iItem;
						CurrentServer = (ClServerClass *)it.lParam;
					}
					break;
			}
			break;
		case WM_COMMAND:
			switch (LOWORD (wParam)) {
				case IDC_GETLIST:
					_beginthread (updw, 0, NULL);
					break;
				case IDC_FIND:
					SendData ("nfs:u");
					break;
				case IDC_ADD:
					DialogBox (hinst, MAKEINTRESOURCE (IDD_ADD), dwnd, &DlgAdd);
					break;
				case IDC_DELETE:
					Servers.RemoveServer (CurrentServer);
					Servers.Save ();
					EnableWindow (GetDlgItem (hwndDlg, IDC_DELETE), false);
					EnableWindow (GetDlgItem (hwndDlg, IDC_LAN), false);
					break;
				case IDC_REFRESH:
					_beginthread (upd, 0, NULL);
					break;
				case IDC_LAN:
					if (IsDlgButtonChecked (dwnd, IDC_LAN)) {
						if (!WriteHosts ()) {
							CheckDlgButton (dwnd, IDC_LAN, false);
						}
					} else {
						ClearHosts ();
					}
					EnableWindow (GetDlgItem (dwnd, IDC_SERVERLIST), !IsDlgButtonChecked (hwndDlg, IDC_LAN));
					EnableWindow (GetDlgItem (dwnd, IDC_DELETE), !IsDlgButtonChecked (hwndDlg, IDC_LAN));
					EnableWindow (GetDlgItem (dwnd, IDC_ADD), !IsDlgButtonChecked (hwndDlg, IDC_LAN));
					EnableWindow (GetDlgItem (dwnd, IDC_FIND), !IsDlgButtonChecked (hwndDlg, IDC_LAN));
					EnableWindow (GetDlgItem (dwnd, IDC_REFRESH), !IsDlgButtonChecked (hwndDlg, IDC_LAN));
					EnableWindow (GetDlgItem (dwnd, IDC_GETLIST), !IsDlgButtonChecked (hwndDlg, IDC_LAN));
					break;
				case IDC_HELP:
					ShellExecute(0, 0, "https://github.com/HarpyWar/nfsuserver/issues", 0, 0, SW_SHOW);
					break;
				case IDC_DOWNLOAD:
					ShellExecute(0, 0, "http://nfsug.harpywar.com/download/nfsu_v1.4.zip", 0, 0, SW_SHOW);
					break;
			}
			break;
		case WM_INITDIALOG:
			dwnd = hwndDlg;
			LV_COLUMN LvCol;
			memset (&LvCol, 0, sizeof (LvCol));
			LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;                         // Type of mask
			LvCol.cx = 245;                                                             // width between each coloum
			LvCol.pszText = "Server";                                                   // First Header Text
			hList = GetDlgItem (dwnd, IDC_SERVERLIST);
			SendMessage (hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT); // Set style

			SendMessage (hList, LVM_INSERTCOLUMN, 0, (LPARAM)&LvCol);                   // Insert/Show the coloum

			LvCol.cx = 100;                                                             // width between each coloum
			LvCol.pszText = "Status";                                                   // Next coloum
			SendMessage (hList, LVM_INSERTCOLUMN, 1, (LPARAM)&LvCol);                   // ...
			LvCol.cx = 50;
			LvCol.pszText = "P/R";                                                      //
			SendMessage (hList, LVM_INSERTCOLUMN, 2, (LPARAM)&LvCol);                   // ...
			//IDC_SERVERLIST

			char buf[1024];
			GetPrivateProfileString ("nfsu", "serverip", "", buf, 1024, INI_FILE);
			SetDlgItemText (dwnd, IDC_IP, buf);
			_beginthread (ListenerThread, 0, NULL);
			Servers.Load ();
			_beginthread (upd, 0, NULL);
			Servers.Save ();
			//			_beginthread(SenderThread, 0, NULL);
			break;
	}

	return FALSE;
}

void StartListener() {
	listenerSocket = socket (AF_INET, SOCK_DGRAM, 0);

	if (listenerSocket == INVALID_SOCKET) return;

	int one = 1;

	if (setsockopt (listenerSocket, SOL_SOCKET, SO_BROADCAST, (char *)&one, sizeof (one)) != 0) {
		closesocket (listenerSocket);
		listenerSocket = INVALID_SOCKET;
		return;
	}

	SOCKADDR_IN localsin;
	localsin.sin_family = AF_INET;
	localsin.sin_addr.s_addr = INADDR_ANY;
	localsin.sin_port = htons (10801);

	if (bind (listenerSocket, (SOCKADDR *)&localsin, sizeof (SOCKADDR_IN)) != 0) {
		closesocket (listenerSocket);
		listenerSocket = INVALID_SOCKET;
	}
}

threadfunc AcceptThread( void *Dummy ) {
	SOCKET cl;
	char log[1024];

	ConAccParam * acc = (ConAccParam *)Dummy;

	unsigned long remote_sockaddr_length = sizeof (SOCKADDR_IN);

	ConnectionClass * temp;

#ifdef _WIN32

	int timeo = 10;
#else

	struct timeval timeo;
	timeo.tv_sec = 0;
	timeo.tv_usec = 10000;
#endif

	while (true) {
		cl = accept (acc->sock, NULL, NULL);
		if (cl != INVALID_SOCKET) {
			temp = (ConnectionClass *)calloc (1, sizeof (ConnectionClass));
			temp->IncomingMessages.mut.Init ();
			temp->OutgoingMessages.mut.Init ();
			temp->sock = cl;

#ifndef _WIN32

			getsockname (temp->sock, (SOCKADDR *)&temp->local_ip, (socklen_t *)&remote_sockaddr_length);
			getpeername (temp->sock, (SOCKADDR *)&temp->remote_ip, (socklen_t *)&remote_sockaddr_length);
#else

			getsockname (temp->sock, (SOCKADDR *)&temp->local_ip, (int *)&remote_sockaddr_length);
			getpeername (temp->sock, (SOCKADDR *)&temp->remote_ip, (int *)&remote_sockaddr_length);
#endif

			time (&temp->Idle);
			acc->Connections->AddConnection (temp);
			temp->id = ids++;
		}
	}
} ;

threadfunc IOThread( void *Dummy ) {
	char log[1024];
	MessageClass * msg;
	ConnectionClass * temp;
	ConnectionsClass * con = (ConnectionsClass *)Dummy;
	int k;
	fd_set check;
	timeval tim;
	tim.tv_sec = 0;
	tim.tv_usec = 100;
	GameClass * Game;
	RoomClass * Room;

	while (true) {
		temp = con->First;

		while (temp != NULL) {
			if (temp->Abort) {
				con->RemoveConnection (temp);
				temp->IncomingMessages.Clear ();
				temp->IncomingMessages.mut.DeInit ();
				temp->OutgoingMessages.Clear ();
				temp->OutgoingMessages.mut.DeInit ();
				closesocket (temp->sock);
				free (temp);
				temp = NULL;
				break;
			}

			msg = temp->OutgoingMessages.RemoveFirstMessage ();

			while (msg != NULL) {
				k = send (temp->sock, msg->Message, msg->Size, 0);

				if (k != msg->Size) {
					closesocket (temp->sock);
					temp->Abort = true;
					break;
				}

				free (msg->Message);
				free (msg);
				msg = temp->OutgoingMessages.RemoveFirstMessage ();
			}

			FD_ZERO (&check);
			FD_SET (temp->sock, &check);
			tim.tv_sec = 0;
			tim.tv_usec = 0;
			k = select (temp->sock + 1, &check, NULL, NULL, &tim);

			if (k < 0) {
				temp->Abort = true;
			}

			if (k == 1) {
				if (temp->Received < 12) {
					k = recv (temp->sock, temp->Buffer + temp->Received, 12 - temp->Received, 0);
					if (k < 1) {
						temp->Abort = true;
					} else {
						temp->Received += k;
					}
				}
				if (temp->Received > 11) {
					DWORD req = (temp->Buffer[11] & 0xFF) | ((temp->Buffer[10] << 8) & 0xFF00)
									| ((temp->Buffer[9] << 16) & 0xFF0000) | ((temp->Buffer[8] << 24) & 0xFF000000);

					if ((req > 12) && (req < 1000)) {
						k = recv (temp->sock, temp->Buffer + temp->Received, req - temp->Received, 0);
						if (k < 1) {
							closesocket (temp->sock);
							temp->Abort = true;
						} else {
							temp->Received += k;
						}
					}

					if (req > 999) {
						temp->Abort = true;
					}
					if (temp->Received == req) {
						msg = (MessageClass *)calloc (1, sizeof (MessageClass));
						msg->Message = (char *)calloc (temp->Received, sizeof (char));
						msg->Size = temp->Received;
						memcpy (msg->Message, temp->Buffer, temp->Received);
						temp->IncomingMessages.AddMessage (msg);
						temp->Received = 0;
					}
				}
			}

			if ((int) (difftime (curtime, temp->Idle)) == 50) {
				temp->Abort = true;
			}
			temp = temp->Next;
		}
		Sleep (1);
	}
} ;

threadfunc RedirectorWorker( void *Dummy ) {
	char log[1024];
	char buffer[1024];

	MessageClass * msg;
	MessageClass * tmsg;

	char * arr[30];
	char arr2[30][1024];

	int k;

	for (k = 0; k < 30; k++)
		arr[k] = (char *)&arr2[k];

	ConnectionClass * temp;

	while (true) {
		RedirectConnections.mut.Lock ();
		temp = RedirectConnections.First;

		while (temp != NULL) {
			msg = temp->IncomingMessages.RemoveFirstMessage ();

			while (msg != NULL) {
				free (msg->Message);
				free (msg);

				if (CurrentServer != NULL) {
					sprintf (arr2[0], "ADDR=%s", CurrentServer->IP);
					sprintf (arr2[1], "PORT=10901");
					sprintf (arr2[2], "SESS=1072010288");
					sprintf (arr2[3], "MASK=0295f3f70ecb1757cd7001b9a7a5eac8");
					k = MakeCommand (buffer, "@dir", arr, 4);
					tmsg = (MessageClass *)calloc (1, sizeof (MessageClass));
					tmsg->Message = (char *)calloc (k, sizeof (char));
					memcpy (tmsg->Message, buffer, k);
					tmsg->Size = k;
					temp->OutgoingMessages.AddMessage (tmsg);
				}
				msg = temp->IncomingMessages.RemoveFirstMessage ();
			}
			temp = temp->Next;
		}

		RedirectConnections.mut.Unlock ();
		Sleep (1);
	}
} ;

//watches for timeouted clients
threadfunc Maintenance(void *Dummy){
	while(true){
		Sleep(1000);
		time(&curtime);
	}
	RETURNFROMTHREAD;
};

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {
	OSVERSIONINFO ver;
	ver.dwOSVersionInfoSize = sizeof ver;
	GetVersionEx (&ver);

	if (ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
		GetWindowsDirectory (fname, 1024);
		strcat (fname, "\\hosts");
	} else {
		GetSystemDirectory (fname, 1024);
		strcat (fname, "\\drivers\\etc\\hosts");
	}

	//Initialise the winsock stack
	WORD wVersionRequested = MAKEWORD (2, 2);
	WSADATA wsaData;
	int err = WSAStartup (wVersionRequested, &wsaData);

	if (err != 0) {
		MessageBox (0, "Could not initialize Winsocks2", "", MB_OK);
		return 0;
	}

	if (strlen (lpCmdLine) > 0) {
		CurrentServer = (ClServerClass *)calloc (1, sizeof (ClServerClass));
		strcpy (CurrentServer->IP, lpCmdLine);

		if (WriteHosts ()) {
			if (!IsSpeedOnline ()) {
				RunSpeed ();
				Sleep (10000);
			}

			//wait
			while (IsSpeedOnline ())
				Sleep (1000);
			ClearHosts ();
		} else {
			MessageBox (0, "Server down or hosts file write protected.", "Error", MB_OK);
		}

		free (CurrentServer);
		return 2;
	}

	hinst = hInstance;
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwICC = ICC_LISTVIEW_CLASSES;
	InitCtrls.dwSize = sizeof (INITCOMMONCONTROLSEX);

	if (!InitCommonControlsEx (&InitCtrls)) {
		MessageBox (0, "Could not initialize CommonControls", "Error", MB_OK);
		return 1;
	}

	//creating own redirector
	time (&curtime);
	RedirectSocket = socket (AF_INET, SOCK_STREAM, 0);

	//binding them to specific ports
	SOCKADDR_IN localsin;
	localsin.sin_family = AF_INET;
	localsin.sin_addr.s_addr = INADDR_ANY;
	localsin.sin_port = htons (10900);
	int k = bind (RedirectSocket, (SOCKADDR *)&localsin, sizeof (SOCKADDR_IN));

	if (k == INVALID_SOCKET) {
		MessageBox (0, "Could not initialize redirect socket\nMaybe you have server started on this box.", "Error", MB_OK);
		closesocket(RedirectSocket);
		RedirectSocket=INVALID_SOCKET;
	}else{
		//listen to ports
		listen (RedirectSocket, 100);
		strcpy (RedirectConnections.Name, "RedirectConnections");
		ConAccParam Redirector;
		Redirector.Connections = &RedirectConnections;
		Redirector.sock = RedirectSocket;
		strcpy (Redirector.Name, "Redirector");
		_beginthread(AcceptThread, 0, (void *)&Redirector);
		_beginthread(IOThread, 0, (void *)&RedirectConnections);
		_beginthread(RedirectorWorker, 0, NULL);
		_beginthread(Maintenance, 0, NULL);
	}

	StartListener ();

	if (listenerSocket == INVALID_SOCKET) {
		MessageBox (0, "Could not bind listenerSocket", "", MB_OK);
		return 0;
	}

	dwnd = CreateDialog (hInstance, MAKEINTRESOURCE (IDD_MAIN), NULL, &MainDlgProc);

	if (dwnd == NULL) {
		MessageBox (0, "Could not create window", "", MB_OK);
	}

	ShowWindow (dwnd, SW_SHOW);
	BOOL bRet;
	MSG msg;

	while ((bRet = GetMessage (&msg, NULL, 0, 0)) != 0) {
		if (bRet != -1) {
			if (!IsWindow (dwnd) || !IsDialogMessage (dwnd, &msg)) {
				TranslateMessage (&msg);
				DispatchMessage (&msg);
			}
		}
	}

	DestroyWindow (dwnd);

	if (listenerSocket != INVALID_SOCKET) closesocket (listenerSocket);

	WSACleanup ();
	return 0;
}

//HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Services\Tcpip\Parameters
