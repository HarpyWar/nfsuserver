#include "win_nix.h"
#include "objects.h"

extern void Log(char *log);
extern bool Verbose;

extern char *arr[30];
extern char arr2[30][1024];

UserClass::UserClass() {
	memset (Username, 0, sizeof (Username));
	memset (Personas, 0, sizeof (Personas));
	memset (IP, 0, 20);
	memset (Port, 0, 10);
	SelectedPerson = -1;
	Idle = 0;
} ;

ServerClass::ServerClass() {
	time (&Startup);
	memset (&ru, 0, sizeof (ru));
	LoadSettings ();
} ;

UsersClass::UsersClass() {
	cid = 1;
	Count = 0;
	First = NULL;
} ;

void UserClass::SelectPerson( char *person ) {
	int k;
	k = 0;

	while (Personas[k][0] != 0) {
		if (strcmp (Personas[k], person) == 0) {
			SelectedPerson = k;
			return;
		}
		k++;
	}
} ;

UserClass *UsersClass::UserFromUsername( char *username ) {
	UserClass * tmp;
	tmp = First;

	while (tmp != NULL) {
		if ((strcmp (tmp->Username, username) == 0) || (strcmp (tmp->Personas[tmp->SelectedPerson], username) == 0)) {
			return tmp;
		}
		tmp = tmp->Next;
	}

	return NULL;
} ;

void UsersClass::AddUser( UserClass *user ) {
	memset (user->car, 0, 1024);
	user->Game = NULL;

	if (user->id == -1) user->id = cid++;

	Count++;
	user->Next = NULL;
	user->Idle = 0;

	if (First == NULL) {
		First = user;
		return;
	}

	UserClass * tmp;
	tmp = First;

	while (tmp->Next != NULL)
		tmp = tmp->Next;

	tmp->Next = user;
}

RUsersClass::RUsersClass() {
	First = NULL;
	Count = 0;
}

void RUsersClass::AddUser( RUserClass *user ) {
	Count++;
	user->Next = NULL;

	if (First == NULL) {
		First = user;
		return;
	}

	RUserClass * tmp;
	tmp = First;

	while (tmp->Next != NULL)
		tmp = tmp->Next;

	tmp->Next = user;
}

void RUsersClass::RemoveUser( RUserClass *user ) {
	if (Count == 1) {
		First = NULL;
	} else {
		if (First == user) {
			First = First->Next;
		} else {
			RUserClass * tmp, * prev;
			tmp = First;
			prev = tmp;

			while ((tmp != user) && (tmp != NULL)) {
				prev = tmp;
				tmp = tmp->Next;
			}
			if (tmp != NULL) {
				prev->Next = tmp->Next;
			}
		}
	}

	Count--;
}

void UsersClass::RemoveUser( UserClass *user ) {
	if (Count == 1) {
		First = NULL;
	} else {
		if (First == user) {
			First = First->Next;
		} else {
			UserClass * tmp, * prev;
			tmp = First;
			prev = tmp;

			while ((tmp != user) && (tmp != NULL)) {
				prev = tmp;
				tmp = tmp->Next;
			}
			if (tmp != NULL) {
				prev->Next = tmp->Next;
			}
		}
	}

	Count--;
}

void ServerClass::SendRoomsToUser( UserClass *user, char *buffer ) {
	RoomClass * tmp;
	tmp = Rooms.First;
	while (tmp != NULL) {
		if ((tmp->Count > 0) || (tmp->IsGlobal)) {
			sprintf (arr2[0], "I=%u", tmp->ID);
			sprintf (arr2[1], "N=%s", tmp->Name);
			sprintf (arr2[2], "H=3PriedeZ");
			sprintf (arr2[3], "F=CK");
			sprintf (arr2[4], "T=%u", tmp->Count);
			sprintf (arr2[5], "L=50");
			sprintf (arr2[6], "P=0");
			if (user->Connection != NULL)
				user->Connection->OutgoingMessages.AddMessage (MakeMessage (buffer, "+rom", arr, 7));
		} else {
			sprintf (buffer, "Error:Room empty\n");
			if (Verbose) Log (buffer);
		}
		tmp = tmp->Next;
	}
} ;

bool ServerClass::RegisterUser( char *username ) {
	RegUser * temp;
	temp = ru.UserFromUsername (username);

	if (temp == NULL) {
		temp = (RegUser *)calloc (1, sizeof (RegUser));
		strcpy (temp->Username, username);
		ru.AddUser (temp);
		return true;
	} else {
		return false;
	} ;
} ;

RoomsClass::RoomsClass() {
	cid = 1;
	Count = 0;
}

void BroadCastCommand( RUsersClass&users, char *command, char *params [], int paramcount, char *buffer ) {
	RUserClass * tmp = users.First;

	while (tmp != NULL) {
		if (tmp->User->Connection != NULL) {
			tmp->User->Connection->OutgoingMessages.AddMessage (MakeMessage (buffer, command, params, paramcount));
		}
		tmp = tmp->Next;
	}
} ;

void BroadCastCommand( UsersClass *users, char *command, char *params [], int paramcount, char *buffer ) {
	UserClass * tmp = users->First;

	while (tmp != NULL) {
		if (tmp->Connection != NULL) {
			tmp->Connection->OutgoingMessages.AddMessage (MakeMessage (buffer, command, params, paramcount));
		}
		tmp = tmp->Next;
	}
} ;

void RoomsClass::AddRoom( RoomClass *room ) {
	Count++;
	room->Next = NULL;
	room->Count = 0;
	room->ID = cid++;

	if (First == NULL) {
		First = room;
		return;
	}

	RoomClass * tmp;
	tmp = First;

	while (tmp->Next != NULL)
		tmp = tmp->Next;

	tmp->Next = room;
}

void RoomsClass::RemoveRoom( RoomClass *room ) {
	if (Count == 1) {
		First = NULL;
	} else {
		if (First == room) {
			First = First->Next;
		} else {
			RoomClass * tmp, * prev;
			tmp = First;
			prev = tmp;

			while ((tmp != room) && (tmp != NULL)) {
				prev = tmp;
				tmp = tmp->Next;
			}
			if (tmp != NULL) {
				prev->Next = tmp->Next;
			}
		}
	}

	Count--;
	room->Name[0] = 0;
}

GameClass *GamesClass::GameFromName( char *name ) {
	GameClass * tmp;
	tmp = First;

	while (tmp != NULL) {
		if (strcmp (tmp->Name, name) == 0) {
			return tmp;
		}
		tmp = tmp->Next;
	}

	return NULL;
} ;
/*
void SessionsClass::AddSession(SessionClass *session){
	Count++;
	session->Next=NULL;
	memset(session->SessionID, '0', 19);
	session->SessionID[19]=0;
	itoa(rand(), session->SessionID, 16);

	if(First==NULL){
		First=session;		
		return;
	}
	SessionClass *tmp;
	tmp=First;
	while(tmp->Next!=NULL) tmp=tmp->Next;
	tmp->Next=session;
};

void SessionsClass::RemoveSession(SessionClass *session){
	if(Count==1){
		First=NULL;
	}else{
		SessionClass *tmp, *prev;
		tmp=First;
		prev=tmp;
		while((tmp!=session)&&(tmp!=NULL)){
			prev=tmp;
			tmp=tmp->Next;
		}
		if(tmp!=NULL){
			prev->Next=tmp->Next;
		}
	}
	Count--;
};

SessionClass* SessionsClass::SessionFromID(char *SessionID){
	SessionClass *tmp;
	tmp=First;
	while(tmp!=NULL){
		if(strcmp(tmp->SessionID, SessionID)==0) return tmp;
		tmp=tmp->Next;
	}
	return NULL;
};

SessionsClass::SessionsClass(){
	First=0;
	Count=0;
};*/

void GamesClass::AddGame( GameClass *game ) {
	Count++;

	if (cid < 1) cid = 1;

	game->Next = NULL;
	game->Count = 0;
	game->ID = cid++;

	if (First == NULL) {
		First = game;
		return;
	}

	GameClass * tmp;
	tmp = First;

	while (tmp->Next != NULL)
		tmp = tmp->Next;

	tmp->Next = game;
}

void GamesClass::RemoveGame( GameClass *game ) {
	if (Count == 1) {
		First = NULL;
	} else {
		if (First == game) {
			First = First->Next;
		} else {
			GameClass * tmp, * prev;
			tmp = First;
			prev = tmp;

			while ((tmp != game) && (tmp != NULL)) {
				prev = tmp;
				tmp = tmp->Next;
			}
			if (tmp != NULL) {
				prev->Next = tmp->Next;
			}
		}
	}

	Count--;
	game->Name[0] = 0;
}

void UsersClass::Broadcast( char *buf, unsigned int len ) {
	UserClass * tmp;
	tmp = First;

	while (tmp != NULL) {
		MessageClass * msg = (MessageClass *)calloc (1, sizeof (MessageClass));
		msg->Message = (char *)calloc (len, sizeof (char));
		msg->Size = len;
		tmp->Connection->OutgoingMessages.AddMessage (msg);
		tmp = tmp->Next;
	}
} ;

RoomClass *RoomsClass::RoomFromName( char *name ) {
	RoomClass * tmp;
	tmp = First;

	while (tmp != NULL) {
		if (strcmp (tmp->Name, name) == 0) return tmp;
		tmp = tmp->Next;
	}

	return NULL;
} ;

void GameClass::SendInfoToUser( UserClass *user, char *buffer ) {
	sprintf (buffer, "GameClass::SendInfoToUser\n");

	if (Verbose) Log (buffer);

	if (Users.Count == 0) {
		sprintf (buffer, "Error: no users in game\n");

		if (Verbose) Log (buffer);
		return;
	}

	int k;

	sprintf (arr2[0], "IDENT=%u", ID);
	sprintf (arr2[1], "WHEN=2003.12.8 15:52:54");
	sprintf (arr2[2], "NAME=%s", Name);
	sprintf (arr2[3], "HOST=%s", Users.First->User->Personas[Users.First->User->SelectedPerson]);
	sprintf (arr2[4], "PARAMS=%s", params);
	sprintf (arr2[5], "ROOM=%s", user->CurrentRoom->Name);
	sprintf (arr2[6], "MAXSIZE=%u", max);
	sprintf (arr2[7], "MINSIZE=%u", min);
	sprintf (arr2[8], "COUNT=%u", Count);
	sprintf (arr2[9], "USERFLAGS=0");
	sprintf (arr2[10], "SYSFLAGS=%u", sysflags);

	k = rand ();
	int l = 0;
	int m = 0;
	RUserClass * ru = Users.First;

	while (ru != NULL) {
		sprintf (arr2[11 + l * 3], "OPID%u=%u", l, ru->User->id);
		sprintf (arr2[12 + l * 3], "OPPO%u=%s", l, ru->User->Personas[ru->User->SelectedPerson]);
		sprintf (arr2[13 + l * 3], "ADDR%u=%s", l, ru->User->IP);
		l++;
		ru = ru->Next;
	}

	if (user->Connection != NULL)
		user->Connection->OutgoingMessages.AddMessage (MakeMessage (buffer, "+agm", arr, 11 + l * 3));
} ;

void GameClass::RemoveUser( UserClass *user, char *buffer ) {
	RUserClass * tmp;
	tmp = Users.First;

	while (tmp != NULL) {
		if (tmp->User == user) {
			Users.RemoveUser (tmp);
			Count--;
			sprintf (buffer, "User removed - %u users in game\n", Count);

			if (Verbose) Log (buffer);

			user->Game = NULL;

			int k;

			if (Count > 0) {
				sprintf (arr2[0], "IDENT=%u", ID);
				sprintf (arr2[1], "WHEN=2003.12.8 15:52:54");
				sprintf (arr2[2], "NAME=%s", Name);
				sprintf (arr2[3], "HOST=%s", Users.First->User->Personas[Users.First->User->SelectedPerson]);
				sprintf (arr2[4], "PARAMS=%s", params);
				sprintf (arr2[5], "ROOM=%s", user->CurrentRoom->Name);
				sprintf (arr2[6], "MAXSIZE=%u", max);
				sprintf (arr2[7], "MINSIZE=%u", min);
				sprintf (arr2[8], "COUNT=%u", Count);
				sprintf (arr2[9], "USERFLAGS=0");
				sprintf (arr2[10], "SYSFLAGS=%u", sysflags);

				k = rand ();
				int l = 0;
				int m = 0;
				RUserClass * ru = Users.First;

				while (ru != NULL) {
					sprintf (arr2[11 + l * 3], "OPID%u=%u", l, ru->User->id);
					sprintf (arr2[12 + l * 3], "OPPO%u=%s", l, ru->User->Personas[ru->User->SelectedPerson]);
					sprintf (arr2[13 + l * 3], "ADDR%u=%s", l, ru->User->IP);
					l++;
					ru = ru->Next;
				}

				if (user->Connection != NULL) {
					user->Connection->OutgoingMessages.AddMessage (MakeMessage (buffer, "glea", arr, 11 + l * 3));
					user->CurrentRoom->RefreshUser (user, buffer);
				}

				if (max == Users.Count) {
					StartGame (buffer);
				}

				sprintf (arr2[0], "IDENT=%u", ID);
				sprintf (arr2[1], "WHEN=2003.12.8 15:52:54");
				sprintf (arr2[2], "NAME=%s", Name);
				sprintf (arr2[3], "HOST=%s", Users.First->User->Personas[Users.First->User->SelectedPerson]);
				sprintf (arr2[4], "PARAMS=%s", params);
				sprintf (arr2[5], "ROOM=%s", user->CurrentRoom->Name);
				sprintf (arr2[6], "MAXSIZE=%u", max);
				sprintf (arr2[7], "MINSIZE=%u", min);
				sprintf (arr2[8], "COUNT=%u", Count);
				sprintf (arr2[9], "USERFLAGS=0");
				sprintf (arr2[10], "SYSFLAGS=%u", sysflags);

				l = 0;
				m = 0;
				ru = Users.First;

				while (ru != NULL) {
					sprintf (arr2[11 + l * 3], "OPID%u=%u", l, ru->User->id);
					sprintf (arr2[12 + l * 3], "OPPO%u=%s", l, ru->User->Personas[ru->User->SelectedPerson]);
					sprintf (arr2[13 + l * 3], "ADDR%u=%s", l, ru->User->IP);
					l++;
					ru = ru->Next;
				}

				BroadCastCommand (user->CurrentRoom->Users, "+agm", arr, 11 + l * 3, buffer);
			} else {
				sprintf (arr2[0], "IDENT=%u", ID);
				BroadCastCommand (user->CurrentRoom->Users, "+agm", arr, 1, buffer);
			}

			free (tmp);
			tmp = NULL;

			return;
		} else {
			tmp = tmp->Next;
		}
	}
} ;

void GameClass::AddUser( UserClass *user, char *buffer ) {
	sprintf (buffer, "Add user\n");

	if (Verbose) Log (buffer);

	user->Game = this;
	Count++;
	RUserClass * tm = (RUserClass *)calloc (1, sizeof (RUserClass));
	tm->User = user;
	Users.AddUser (tm);

	int k;

	sprintf (arr2[0], "IDENT=%u", ID);
	sprintf (arr2[1], "WHEN=2003.12.8 15:52:54");
	sprintf (arr2[2], "NAME=%s", Name);
	sprintf (arr2[3], "HOST=%s", Users.First->User->Personas[Users.First->User->SelectedPerson]);
	sprintf (arr2[4], "PARAMS=%s", params);
	sprintf (arr2[5], "ROOM=%s", user->CurrentRoom->Name);
	sprintf (arr2[6], "MAXSIZE=%u", max);
	sprintf (arr2[7], "MINSIZE=%u", min);
	sprintf (arr2[8], "COUNT=%u", Count);
	sprintf (arr2[9], "USERFLAGS=0");
	sprintf (arr2[10], "SYSFLAGS=%u", sysflags);

	k = rand ();
	int l = 0;
	int m = 0;
	RUserClass * ru = Users.First;

	while (ru != NULL) {
		sprintf (arr2[11 + l * 3], "OPID%u=%u", l, ru->User->id);
		sprintf (arr2[12 + l * 3], "OPPO%u=%s", l, ru->User->Personas[ru->User->SelectedPerson]);
		sprintf (arr2[13 + l * 3], "ADDR%u=%s", l, ru->User->IP);
		l++;
		ru = ru->Next;
	}

	if (user->Connection) user->Connection->OutgoingMessages.AddMessage (MakeMessage (buffer, "gjoi", arr, 11 + l * 3));

	user->CurrentRoom->RefreshUser (user, buffer);

	if (max == Users.Count) {
		StartGame (buffer);
	}


	sprintf (arr2[0], "IDENT=%u", ID);
	sprintf (arr2[1], "WHEN=2003.12..8 15:52:54");
	sprintf (arr2[2], "NAME=%s", Name);
	sprintf (arr2[3], "HOST=%s", Users.First->User->Personas[Users.First->User->SelectedPerson]);
	sprintf (arr2[4], "PARAMS=%s", params);
	sprintf (arr2[5], "ROOM=%s", user->CurrentRoom->Name);
	sprintf (arr2[6], "MAXSIZE=%u", max);
	sprintf (arr2[7], "MINSIZE=%u", min);
	sprintf (arr2[8], "COUNT=%u", Count);
	sprintf (arr2[9], "USERFLAGS=0");
	sprintf (arr2[10], "SYSFLAGS=%u", sysflags);

	l = 0;
	m = 0;
	ru = Users.First;

	while (ru != NULL) {
		sprintf (arr2[11 + l * 3], "OPID%u=%u", l, ru->User->id);
		sprintf (arr2[12 + l * 3], "OPPO%u=%s", l, ru->User->Personas[ru->User->SelectedPerson]);
		sprintf (arr2[13 + l * 3], "ADDR%u=%s", l, ru->User->IP);
		l++;
		ru = ru->Next;
	}
	BroadCastCommand (user->CurrentRoom->Users, "+agm", arr, 11 + l * 3, buffer);
}

void GameClass::StartGame( char *buffer ) {
	sprintf (buffer, "start game\n");

	if (Verbose) Log (buffer);

	int k;

	sprintf (arr2[0], "IDENT=%u", ID);
	sprintf (arr2[1], "WHEN=2003.12.8 15:52:54");
	sprintf (arr2[2], "NAME=%s", Name);
	sprintf (arr2[3], "HOST=%s", Users.First->User->Personas[Users.First->User->SelectedPerson]);
	sprintf (arr2[4], "PARAMS=%s", params);
	sprintf (arr2[5], "ROOM=%s", Users.First->User->CurrentRoom->Name);
	sprintf (arr2[6], "MAXSIZE=%u", max);
	sprintf (arr2[7], "MINSIZE=%u", min);
	sprintf (arr2[8], "COUNT=%u", Count);
	sprintf (arr2[9], "USERFLAGS=0");
	sprintf (arr2[10], "SYSFLAGS=%u", sysflags);

	k = rand ();
	int l = 0;
	int m = 0;
	RUserClass * ru = Users.First;

	while (ru != NULL) {
		sprintf (arr2[11 + l * 3], "OPID%u=%u", l, ru->User->id);
		sprintf (arr2[12 + l * 3], "OPPO%u=%s", l, ru->User->Personas[ru->User->SelectedPerson]);
		sprintf (arr2[13 + l * 3], "ADDR%u=%s", l, ru->User->IP);
		l++;
		ru = ru->Next;
	}

	ru = Users.First;

	while (ru != NULL) {
		sprintf (arr2[11 + l * 3], "SEED=%u", k);
		sprintf (arr2[12 + l * 3], "SELF=%s", ru->User->Personas[ru->User->SelectedPerson]);

		if (ru->User->Connection != NULL)
			ru->User->Connection->OutgoingMessages.AddMessage (MakeMessage (buffer, "+ses", arr, 13 + l * 3));
		ru = ru->Next;
	}

	BroadCastCommand (Users.First->User->CurrentRoom->Users, "+agm", arr, 11 + l * 3, buffer);
}

void RoomClass::AddUser( UserClass *user, char *buffer ) {
	if (Verbose) 
		Log ("Add user\n");

	sprintf (arr2[0], "IDENT=%u", ID);
	sprintf (arr2[1], "NAME=%s", Name);
	sprintf (arr2[2], "COUNT=%u", Users.Count);
	sprintf (arr2[3], "FLAGS=C");

	if (user->Connection != NULL) user->Connection->OutgoingMessages.AddMessage (MakeMessage (buffer, "move", arr, 4));

	user->CurrentRoom = this;
	Count++;
	RUserClass * tm = (RUserClass *)calloc (1, sizeof (RUserClass));
	tm->User = user;

	Users.AddUser (tm);

	sprintf (buffer, "User added - %u users in room\n", Count);

	if (Verbose) Log (buffer);

	sprintf (arr2[0], "I=%u", user->id);
	sprintf (arr2[1], "N=%s", user->Personas[user->SelectedPerson]);
	sprintf (arr2[2], "M=%s", user->Username);
	sprintf (arr2[3], "F=");
	sprintf (arr2[4], "A=%s", user->IP);
	sprintf (arr2[5], "S=");
	sprintf (arr2[6], "X=%s", user->car);
	sprintf (arr2[7], "R=%s", Name);
	sprintf (arr2[8], "RI=%u", ID);
	sprintf (arr2[9], "RF=C");
	sprintf (arr2[10], "RT=1");

	if (user->Connection != NULL) user->Connection->OutgoingMessages.AddMessage (MakeMessage (buffer, "+who", arr, 11));

	RefreshUser (user, buffer);

	GameClass * game = Games.First;

	while (game != NULL) {
		game->SendInfoToUser (user, buffer);
		game = game->Next;
	}

	ListToUser (user, buffer);
} ;

void RoomClass::RefreshUser( UserClass *user, char *buffer ) {
	sprintf (buffer, "RoomClass::RefreshUser\n");

	if (Verbose) Log (buffer);

	sprintf (arr2[0], "I=%u", user->id);
	sprintf (arr2[1], "N=%s", user->Personas[user->SelectedPerson]);
	sprintf (arr2[2], "M=%s", user->Username);
	sprintf (arr2[3], "F=H");
	sprintf (arr2[4], "A=%s", user->IP);
	sprintf (arr2[5], "P=211");
	sprintf (arr2[6], "S=");
	sprintf (arr2[7], "X=%s", user->car);

	if (user->Game != NULL) {
		sprintf (arr2[8], "G=%u", user->Game->ID);
	} else {
		sprintf (arr2[8], "G=0");
	}

	sprintf (arr2[9], "T=2");

	BroadCastCommand (Users, "+usr", arr, 10, buffer);
} ;

void RoomClass::RemoveUser( UserClass *user, char *buffer ) {
	RUserClass * tmp;
	tmp = Users.First;

	while (tmp != NULL) {
		if (tmp->User == user) {
			Users.RemoveUser (tmp);
			user->CurrentRoom = NULL;
			Count--;
			sprintf (buffer, "User removed - %u users in room\n", Count);

			if (Verbose) Log (buffer);

			sprintf (arr2[0], "I=%u", user->id);
			sprintf (arr2[1], "T=1");

			BroadCastCommand (Users, "+usr", arr, 2, buffer);

			sprintf (arr2[0], "F=C");
			sprintf (arr2[1], "T=\"has left the room\"");
			sprintf (arr2[2], "N=%s", user->Personas[user->SelectedPerson]);

			BroadCastCommand (Users, "+msg", arr, 3, buffer);

			free (tmp);
			tmp = NULL;
			return;
		} else {
			tmp = tmp->Next;
		}
	}
} ;

void RoomClass::ListToUser( UserClass *user, char *buffer ) {
	RUserClass * tmp;
	tmp = Users.First;
	sprintf (buffer, "Sending list\n");

	if (Verbose) Log (buffer);

	while (tmp != NULL) {
		sprintf (arr2[0], "I=%u", tmp->User->id);
		sprintf (arr2[1], "N=%s", tmp->User->Personas[tmp->User->SelectedPerson]);
		sprintf (arr2[2], "M=%s", tmp->User->Username);
		sprintf (arr2[3], "F=H");
		sprintf (arr2[4], "A=%s", tmp->User->IP);
		sprintf (arr2[5], "P=211");
		sprintf (arr2[6], "S=");
		sprintf (arr2[7], "X=%s", tmp->User->car);

		if (tmp->User->Game != NULL) {
			sprintf (arr2[8], "G=%u", tmp->User->Game->ID);
		} else {
			sprintf (arr2[8], "G=0");
		}

		sprintf (arr2[9], "T=2");

		if (user->Connection != NULL)
			user->Connection->OutgoingMessages.AddMessage (MakeMessage (buffer, "+usr", arr, 10));
		tmp = tmp->Next;
	}
} ;

int atoi2( char *str ) {
	int ret = 0;
	char * tmp;
	tmp = str;

	while ((tmp != NULL) && (tmp[0] >= '0') && (tmp[0] <= '9')) {
		ret = ret * 10 + tmp[0] - '0';
		tmp++;
	}

	return ret;
} ;

RegUser *RegUsers::UserFromUsername( char *username ) {
	RegUser * temp = First;

	while (temp != NULL) {
		if (stricmp (temp->Username, username) == 0) {
			return temp;
		}
		temp = temp->Next;
	}

	return NULL;
} ;

void RegUsers::AddUser( RegUser *user ) {
	user->Next = NULL;

	if (Count == 0) {
		First = user;
		Last = user;
	} else {
		Last->Next = user;
		Last = user;
	}

	Count++;
} ;

void RegUsers::RemoveUser( RegUser *user ) {
	if (user == NULL) return;

	if (Count == 0) return;

	if (Count == 1) {
		First = NULL;
		Last = NULL;
		Count = 0;
	} else {
		if (First == user) {
			First = First->Next;
			Count--;
			return;
		}

		RegUser * prev, * temp;
		temp = First;
		prev = NULL;

		while ((temp != user) && (temp != NULL)) {
			prev = temp;
			temp = temp->Next;
		}
		if (prev != NULL) {
			if (temp == Last) {
				Last = prev;
				prev->Next = NULL;
			} else {
				prev->Next = temp->Next;
			}
			Count--;
		}
	}
} ;

void ServerClass::SaveSettings() {
	FILE * fil;
	fil = fopen ("rusers.dat", "wb");

	if (fil == NULL) return;

	RegUser * temp = ru.First;

	while (temp != NULL) {
		fwrite (temp, 1, sizeof (RegUser), fil);
		temp = temp->Next;
	}

	fclose (fil);
} ;
void ServerClass::LoadSettings() {
	while (ru.Count > 0)
		ru.RemoveUser (ru.Last);

	FILE * fil;
	fil = fopen ("rusers.dat", "rb");

	if (fil == NULL) return;

	RegUser * temp;

	while (!feof (fil)) {
		temp = (RegUser *)calloc (1, sizeof (RegUser));
		fread (temp, 1, sizeof (RegUser), fil);
		temp->Next = NULL;
		ru.AddUser (temp);
	}

	fclose (fil);
} ;
