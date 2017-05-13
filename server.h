struct RegUser {
	char Username[100];
	char Personas[4][100];
	RegUser * Next;
};

class RegUsers {
	public:
		int Count;
		RegUser * First;
		RegUser * Last;
		void Load ();
		void Save ();
		RegUser * UserFromUsername (char * username);
		void AddUser (RegUser * user);
	void RemoveUser (RegUser * user);
};

class RoomClass;
class GameClass;
class ConnectionClass;

class UserClass {
	public:
		int Version;
		int id;
		int Rank[4];
		GameClass * Game;
		ConnectionClass * Connection;
		char Username[100];
		char Personas[4][100];
		char car[1024];
		char IP[20];
		char Port[10];
		UserClass ();
		void SelectPerson (char * person);
		int SelectedPerson;
		UserClass * Next;
		RoomClass * CurrentRoom;
	int Idle;
};

class UsersClass {
	public:
		int cid;
		unsigned int Count;
		UserClass * First;
		void AddUser (UserClass * user);
		void RemoveUser (UserClass * user);
		void Broadcast (char *buf, unsigned int len);
		UserClass * UserFromUsername (char * username);
	UsersClass ();
};

class RUserClass {
	public:
		UserClass * User;
	RUserClass * Next;
};

class RUsersClass {
	public:
		unsigned int Count;
		RUserClass * First;
		void AddUser (RUserClass * user);
		void RemoveUser (RUserClass * user);
		void Broadcast (char *buf, unsigned int len);
		RUserClass * UserFromUsername (char * username);
	RUsersClass ();
};

class GameClass {
	public:
		bool working;
		int max, min;
		int Count;
		int ID;
		int sysflags;
		char params[300];
		char Name[200];
		RUsersClass Users;
		void AddUser (UserClass * user, char * buffer);
		void RemoveUser (UserClass * user, char * buffer);
		void Refresh ();
		void SendInfoToUser (UserClass * user, char * buffer);
		void StartGame (char * buffer);
	GameClass * Next;
};

class GamesClass {
	public:
		int cid;
		GameClass * First;
		int Count;
		void AddGame (GameClass * game);
		void RemoveGame (GameClass * game);
		GameClass * GameFromID (int id);
	GameClass * GameFromName (char * name);
};

class RoomClass {
	public:
		RUsersClass Users;
		GamesClass Games;
		int Count;
		int ID;
		bool IsGlobal;
		char Name[100];
		RoomClass * Next;
		void AddUser (UserClass * user, char * buffer);
		void RemoveUser (UserClass * user, char * buffer);
		void ListToUser (UserClass * user, char * buffer);
		void RefreshUser (UserClass * user, char * buffer);
};

class RoomsClass {
	public:
		int cid;
		int Count;
		RoomClass * First;
		void AddRoom (RoomClass * room);
		void RemoveRoom (RoomClass * room);
		RoomClass * RoomFromName (char * name);
	RoomsClass ();
};
/*
class SessionClass{
public:
	UserClass *User;
	char SessionID[20];
	//void RemoveUserFromRoomsAndGames();
	SessionClass *Next;
};

class SessionsClass{
public:
	SessionClass *First;
	int Count;
	bool Updating;
	void AddSession(SessionClass *session);
	void RemoveSession(SessionClass *session);
	SessionClass* SessionFromID(char *SessionID);
	SessionsClass();
};
*/
class ServerClass {
	public:
		//SessionsClass Sessions;
		char Name[100];
		time_t Startup;
		UsersClass Users;
		RoomsClass Rooms;
		RegUsers ru;
		void SendRoomsToUser (UserClass * user, char * buffer);
		ServerClass ();
		void LoadSettings ();
		void SaveSettings ();
	bool RegisterUser (char * username);
};

bool RecvCommand( SOCKET sock, char *buffer );
bool SendCommand( SOCKET sock, char *command, char *params [], int paramcount );
int MakeCommand( char *buffer, char *command, char *params [], int paramcount );
void BroadCastCommand( UsersClass *users, char *command, char *params [], int paramcount, char *buffer );
void BroadCastCommand( RUsersClass&users, char *command, char *params [], int paramcount, char *buffer );
int recvn( SOCKET sock, char *buf, int required );
int atoi2( char *str );
