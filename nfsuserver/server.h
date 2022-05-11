struct PlayerStat {
	char Name[16];
	int Rating_All;
	int Wins_All;
	int Loses_All;
	int Disc_All;
	int REP_All;
	int OppsREP_All;	// average opponents REP
	int OppsRating_All;	// average opponents Rating
	int Rating_Circ;
	int Wins_Circ;
	int Loses_Circ;
	int Disc_Circ;
	int REP_Circ;
	int OppsREP_Circ;
	int OppsRating_Circ;
	int Rating_Sprint;
	int Wins_Sprint;
	int Loses_Sprint;
	int Disc_Sprint;
	int REP_Sprint;
	int OppsREP_Sprint;
	int OppsRating_Sprint;
	int Rating_Drag;
	int Wins_Drag;
	int Loses_Drag;
	int Disc_Drag;
	int REP_Drag;
	int OppsREP_Drag;
	int OppsRating_Drag;
	int Rating_Drift;
	int Wins_Drift;
	int Loses_Drift;
	int Disc_Drift;
	int REP_Drift;
	int OppsREP_Drift;
	int OppsRating_Drift;

	PlayerStat(void) {}

	PlayerStat(const char* fName,
		const int fRating_All, const int fWins_All, const int fLoses_All, const int fDisc_All, const int fREP_All, const int fOppsREP_All, const int fOppsRating_All,
		const int fRating_Circ, const int fWins_Circ, const int fLoses_Circ, const int fDisc_Circ, const int fREP_Circ, const int fOppsREP_Circ, const int fOppsRating_Circ,
		const int fRating_Sprint, const int fWins_Sprint, const int fLoses_Sprint, const int fDisc_Sprint, const int fREP_Sprint, const int fOppsREP_Sprint, const int fOppsRating_Sprint,
		const int fRating_Drag, const int fWins_Drag, const int fLoses_Drag, const int fDisc_Drag, const int fREP_Drag, const int fOppsREP_Drag, const int fOppsRating_Drag,
		const int fRating_Drift, const int fWins_Drift, const int fLoses_Drift, const int fDisc_Drift, const int fREP_Drift, const int fOppsREP_Drift, const int fOppsRating_Drift) {

		strcpy(Name, fName);
		Rating_All = fRating_All;
		Wins_All = fWins_All;
		Loses_All = fLoses_All;
		Disc_All = fDisc_All;
		REP_All = fREP_All;
		OppsREP_All = fOppsREP_All;
		OppsRating_All = fOppsRating_All;
		Rating_Circ = fRating_Circ;
		Wins_Circ = fWins_Circ;
		Loses_Circ = fLoses_Circ;
		Disc_Circ = fDisc_Circ;
		REP_Circ = fREP_Circ;
		OppsREP_Circ = fOppsREP_Circ;
		OppsRating_Circ = fOppsRating_Circ;
		Rating_Sprint = fRating_Sprint;
		Wins_Sprint = fWins_Sprint;
		Loses_Sprint = fLoses_Sprint;
		Disc_Sprint = fDisc_Sprint;
		REP_Sprint = fREP_Sprint;
		OppsREP_Sprint = fOppsREP_Sprint;
		OppsRating_Sprint = fOppsRating_Sprint;
		Rating_Drag = fRating_Drag;
		Wins_Drag = fWins_Drag;
		Loses_Drag = fLoses_Drag;
		Disc_Drag = fDisc_Drag;
		REP_Drag = fREP_Drag;
		OppsREP_Drag = fOppsREP_Drag;
		OppsRating_Drag = fOppsRating_Drag;
		Rating_Drift = fRating_Drift;
		Wins_Drift = fWins_Drift;
		Loses_Drift = fLoses_Drift;
		Disc_Drift = fDisc_Drift;
		REP_Drift = fREP_Drift;
		OppsREP_Drift = fOppsREP_Drift;
		OppsRating_Drift = fOppsRating_Drift;
	}

	bool operator == (const char* hStr) const {
		return (!strcmp(Name, hStr));
	}
};

struct StarsLap {
	char Name[16];
	int Time;
	int Car;
	int Dir;

	StarsLap(void) {}

	StarsLap(const char* fName, const int fTime, const int fCar, const int fDir) {
		strcpy(Name, fName);
		Time = fTime;
		Car = fCar;
		Dir = fDir;
	}

	bool operator == (const char* hStr) const {
		return (!strcmp(Name, hStr));
	}
};

struct StarsDrift {
	char Name[16];
	int Points;
	int Car;
	int Dir;

	StarsDrift(void) {}

	StarsDrift(const char* fName, const int fPoints, const int fCar, const int fDir) {
		strcpy(Name, fName);
		Points = fPoints;
		Car = fCar;
		Dir = fDir;
	}

	bool operator == (const char* hStr) const {
		return (!strcmp(Name, hStr));
	}
};


struct RegUser {
	char Username[16];
	char Personas[4][16];
	RegUser* Next;
};

class RegUsers {
public:
	int Count;
	RegUser* First;
	RegUser* Last;
	RegUser* UserFromUsername(char* username);
	void AddUser(RegUser* user);
	void RemoveUser(RegUser* user);
};

class RoomClass;
class GameClass;
class ConnectionClass;

class UserClass {
public:
	int Version;
	int id;
	int Rank[4];
	GameClass* Game;
	ConnectionClass* Connection;
	char Username[100];
	char Personas[4][100];
	char car[1024];
	char IP[20];
	char Port[10];
	UserClass();
	void SelectPerson(char* person);
	int SelectedPerson;
	UserClass* Next;
	RoomClass* CurrentRoom;
	int Idle;
};

class UsersClass {
public:
	int cid;
	unsigned int Count;
	UserClass* First;
	void AddUser(UserClass* user);
	void RemoveUser(UserClass* user);
	void Broadcast(char* buf, unsigned int len);
	UserClass* UserFromUsername(char* username);
	UsersClass();
};

class RUserClass {
public:
	UserClass* User;
	RUserClass* Next;
};

class RUsersClass {
public:
	unsigned int Count;
	RUserClass* First;
	void AddUser(RUserClass* user);
	void RemoveUser(RUserClass* user);
	RUsersClass();
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
	void AddUser(UserClass* user, char* buffer);
	void RemoveUser(UserClass* user, char* buffer);
	void SendInfoToUser(UserClass* user, char* buffer);
	void StartGame(char* buffer);
	GameClass* Next;
};

class GamesClass {
public:
	int cid;
	GameClass* First;
	int Count;
	void AddGame(GameClass* game);
	void RemoveGame(GameClass* game);
	GameClass* GameFromName(char* name);
};

class RoomClass {
public:
	RUsersClass Users;
	GamesClass Games;
	int Count;
	int ID;
	bool IsGlobal;
	char Name[100];
	RoomClass* Next;
	void AddUser(UserClass* user, char* buffer);
	void RemoveUser(UserClass* user, char* buffer);
	void ListToUser(UserClass* user, char* buffer);
	void RefreshUser(UserClass* user, char* buffer);
};

class RoomsClass {
public:
	int cid;
	int Count;
	RoomClass* First;
	void AddRoom(RoomClass* room);
	void RemoveRoom(RoomClass* room);
	RoomClass* RoomFromName(char* name);
	RoomsClass();
};

class SessionClass{
public:
	char Persona[16];
	char FromRoom[100];
	char IP[20];
	// Idle needs for remove unposted sessions
	int Idle;
	SessionClass* Next;
	char SessionID[20];
};

class SessionsClass{
public:
	SessionClass* First;
	int Count;
	bool Updating;
	void AddSession(SessionClass* session);
	void RemoveSession(SessionClass* session);
	SessionsClass();
};

class ServerClass {
public:
	char Name[100];
	char ServerIP[100];
	char ServerExternalIP[100];
	char WelcomeMessage[100];
	time_t Startup;
	UsersClass Users;
	RoomsClass Rooms;
	RegUsers ru;
	void SendRoomsToUser(UserClass* user, char* buffer);
	ServerClass();
	void LoadSettings();
	void SaveSettings();
	void LoadStat();
	void SaveStat();
	void LoadStars(int track);
	void SaveStars(int track);
	void LoadStarsDrift(int track);
	void SaveStarsDrift(int track);
	bool RegisterUser(char* username);
};

bool RecvCommand(SOCKET sock, char* buffer);
bool SendCommand(SOCKET sock, char* command, char* params[], int paramcount);
int MakeCommand(char* buffer, char* command, char* params[], int paramcount);
void BroadCastCommand(UsersClass* users, char* command, char* params[], int paramcount, char* buffer);
void BroadCastCommand(RUsersClass& users, char* command, char* params[], int paramcount, char* buffer);
int recvn(SOCKET sock, char* buf, int required);
int atoi2(char* str);
char* GetPlayerStat(const char* Name);
void GetOppREP_Rating(const char* name, const char race_type, int& rep, int& rating);
