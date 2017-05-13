#include "servers.h"
#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include <time.h>

extern HWND hList;

void ClServersClass::AddServer( ClServerClass * Server ) {
	Server->Next = NULL;

	if (Count == 0) {
		First = Server;
		Last = Server;
	} else {
		Last->Next = Server;
		Last = Server;
	}

	Count++;
} ;

ClServerClass *ClServersClass::ServerFromIP( unsigned long ip ) {
	ClServerClass *tmp = First;

	while (tmp != NULL) {
		if (tmp->ip == ip) return tmp;
		tmp = tmp->Next;
	}

	return NULL;
} ;

void ClServerClass::Update() {
	LV_ITEM item;
	LVFINDINFO find;

	find.flags = LVFI_PARAM;
	find.lParam = (LPARAM)this;
	item.iItem = ListView_FindItem (hList, -1, &find);

	if (item.iItem == -1) {
		memset (&item, 0, sizeof (item));
		item.mask = LVIF_TEXT | LVIF_PARAM; // Text Style
		item.cchTextMax = 256;              // Max size of test
		item.iItem = 0;                     // choose item 
		item.iSubItem = 0;                  // Put in first coluom
		item.pszText = IP;                  // Text to display (can be from a char variable) (Items)
		item.lParam = (LPARAM)this;
		item.iItem = ListView_InsertItem (hList, &item);
		item.mask = LVIF_TEXT;
		item.iSubItem = 1;
		item.pszText = "Offline";
		ListView_SetItem (hList, &item);
		item.mask = LVIF_TEXT;
		item.iSubItem = 2;
		item.pszText = "-";
		ListView_SetItem (hList, &item);
	} else {
		item.mask = LVIF_TEXT | LVIF_PARAM; // Text Style
		item.iSubItem = 0;                  // Put in first coluom
		item.pszText = IP;                  // Text to display (can be from a char variable) (Items)
		item.lParam = (LPARAM)this;
		ListView_SetItem (hList, &item);
		item.mask = LVIF_TEXT;
		item.iSubItem = 1;
		item.pszText = "Offline";
		ListView_SetItem (hList, &item);
		item.mask = LVIF_TEXT;
		item.iSubItem = 2;
		item.pszText = "-";
		ListView_SetItem (hList, &item);
	}

	Rooms = 0;
	Users = 0;
	TimeOnline = 0;
	Name[0] = 0;
	IsOnline = false;

	struct hostent *hostInfo = gethostbyname (IP);
	SOCKADDR_IN remote_sockaddr_in;
	unsigned long remote_sockaddr_length = sizeof (SOCKADDR_IN);
	int len;
	char buf[1024];
	char *t, * t2;
	int s, m, h;

	remote_sockaddr_in.sin_family = AF_INET;
	remote_sockaddr_in.sin_port = htons (10800);

	if (hostInfo != NULL) {
		memcpy (&remote_sockaddr_in.sin_addr.S_un.S_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
	} else {
		remote_sockaddr_in.sin_addr.s_addr = inet_addr (IP);
		if (remote_sockaddr_in.sin_addr.s_addr == INADDR_NONE) return;
	}

	ip = remote_sockaddr_in.sin_addr.s_addr;

	SOCKET sock = socket (AF_INET, SOCK_STREAM, 0);

	if (sock == INVALID_SOCKET) return;

	if (connect (sock, (const sockaddr *)&remote_sockaddr_in, remote_sockaddr_length) == 0) {
		len = recv (sock, buf, 1024, 0);
		if (len > 0) {
			IsOnline = true;
			buf[len] = 0;
			//sprintf(tempBuff, "%u|%u|%u|%s|%s|%s", 
			//ClientConnections.Count, 
			//Server.Rooms.Count-8, 
			//status, 
			//SERVER_PLATFORM, 
			//NFSU_LAN_VERSION, 
			//Server.Name);
			t2 = strchr (buf, '|');
			t = buf;

			if (t2 != NULL) {
				t2[0] = 0;
				Users = atoi (t);
				t = t2 + 1;
				t2 = strchr (t, '|');
				if (t2 != NULL) {
					t2[0] = 0;
					Rooms = atoi (t);
					t = t2 + 1;
					t2 = strchr (t, '|');
					if (t2 != NULL) {
						t2[0] = 0;
						TimeOnline = atoi (t);
						t = t2 + 1;
						t2 = strchr (t, '|');
						if (t2 != NULL) {
							t2[0] = 0;
							IsWin32 = (t[0] == 'w');
							t = t2 + 1;
							t2 = strchr (t, '|');
							if (t2 != NULL) {
								t2[0] = 0;
								strcpy (Version, t);
								strcpy (Name, t2 + 1);
							}
						}
					}
				}
			}

			char po[100];
			if (item.iItem != -1) {
				item.mask = LVIF_TEXT;
				item.lParam = (LPARAM)this;

				if (IsFav) {
					strcpy (po, "public");
				} else {
					strcpy (po, "local");
				}

				remote_sockaddr_in.sin_addr.S_un.S_addr = ip;

				if (IsWin32) {
					sprintf (buf, "%s [%s:win32:%s:%s]", Name, Version, po, inet_ntoa (remote_sockaddr_in.sin_addr));
				} else {
					sprintf (buf, "%s [%s:*nix:%s:%s]", Name, Version, po, inet_ntoa (remote_sockaddr_in.sin_addr));
				}

				item.pszText = buf;
				item.iSubItem = 0;
				ListView_SetItem (hList, &item);

				s = TimeOnline;
				h = s / 3600; // Find number of hours
				s = s % 3600; // Find remaining seconds
				m = s / 60;   // Find number of minutes
				s = s % 60;   // Find seconds left at end
				sprintf (buf, "Online: %02u:%02u:%02u", h, m, s);
				item.pszText = buf;
				item.iSubItem = 1;
				ListView_SetItem (hList, &item);

				sprintf (buf, "%u/%u", Users, Rooms);
				item.pszText = buf;
				item.iSubItem = 2;
				ListView_SetItem (hList, &item);
			}
		}
	}

	closesocket (sock);
} ;

void ClServersClass::RemoveServer( ClServerClass * Server ) {
	if (Count == 1) {
		First = NULL;
	} else {
		if (First == Server) {
			First = First->Next;
		} else {
			ClServerClass *tmp, * prev;
			tmp = First;
			prev = tmp;

			while ((tmp != Server) && (tmp != NULL)) {
				prev = tmp;
				tmp = tmp->Next;
			}
			if (tmp != NULL) {
				prev->Next = tmp->Next;
			}
		}
	}

	LVFINDINFO find;

	find.flags = LVFI_PARAM;
	find.lParam = (LPARAM)Server;

	int index = ListView_FindItem (hList, -1, &find);
	ListView_DeleteItem (hList, index);

	Count--;
} ;

void ClServersClass::Save() {
	FILE *fil;
	fil = fopen ("servers.dat", "wt");

	if (fil == NULL) return;

	ClServerClass *temp = First;

	while (temp != NULL) {
		if (temp->IsFav) {
			fputs (temp->IP, fil);
			fputs ("\n", fil);
		}
		temp = temp->Next;
	}

	fclose (fil);
} ;

void ClServersClass::Load() {
	FILE *fil;
	fil = fopen ("servers.dat", "rt");

	if (fil == NULL) return;

	char buf[100];
	ClServerClass *temp;

	while (Count > 0)
		RemoveServer (First);

	while (fgets (buf, 100, fil) != NULL) {
		buf[strlen (buf) - 1] = 0;
		temp = (ClServerClass *)calloc (1, sizeof (ClServerClass));
		strcpy (temp->IP, buf);
		temp->IsFav = true;
		AddServer (temp);
	}

	fclose (fil);
} ;

void ClServersClass::Update() {
	ClServerClass *temp = First;

	while (temp != NULL) {
		temp->Update ();
		temp = temp->Next;
	}
} ;
void ClServersClass::UpdateFromWeb() {
	char fname[1024];
	char buf[1024];
	srand (time (NULL));
	sprintf (buf, "http://3priedez.net/nfsug/get_list.py?%u", rand ());

	if (URLDownloadToCacheFile (NULL, buf, fname, 1024, 0, NULL) == S_OK) {
		FILE *fil;
		fil = fopen (fname, "rt");

		if (fil == NULL) return;

		unsigned long ip;
		in_addr an;
		ClServerClass *temp;

		while (fgets (fname, 100, fil) != NULL) {
			fname[strlen (fname) - 1] = 0;
			if (strlen (fname) > 3) {
				ip = atoi (fname);
				if (ServerFromIP (ip) == NULL) {
					temp = (ClServerClass *)calloc (1, sizeof (ClServerClass));
					an.S_un.S_addr = ip;
					strcpy (temp->IP, inet_ntoa (an));
					temp->IsFav = true;
					AddServer (temp);
					temp->Update ();
				}
			}
		}
		fclose (fil);
	}
} ;
