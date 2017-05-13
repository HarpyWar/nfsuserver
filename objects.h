#include "win_nix.h"
#include "mutex.h"
#include "server.h"

class MessageClass {
	public:
		char * Message;
		unsigned int Size;
	MessageClass * Next;
};

class MessagesClass {
	public:
		MessageClass * First;
		MessageClass * Last;
		unsigned int Count;
		MessageClass * RemoveFirstMessage ();
		void Clear ();
		void AddMessage (MessageClass * msg);
	Mutex mut;
};

class ConnectionClass {
	public:
		time_t Idle;
		unsigned int id;
		bool Abort;
		SOCKET sock;
		char Buffer[1024];
		char IP[20];
		char Port[10];
		unsigned long Received;
		UserClass * user;
		SOCKADDR_IN local_ip;
		SOCKADDR_IN remote_ip;
		MessagesClass IncomingMessages;
		MessagesClass OutgoingMessages;
	ConnectionClass * Next;
};

class ConnectionsClass {
	public:
		char Name[100];
		ConnectionClass * First;
		ConnectionClass * Last;
		unsigned int Count;
		void Clear ();
		ConnectionClass * RemoveFirstConnection ();
		void RemoveConnection (ConnectionClass * con);
		void AddConnection (ConnectionClass * con);
		ConnectionsClass ();
		~ConnectionsClass ();
	Mutex mut;
};

struct ConAccParam {
	char Name[100];
	ConnectionsClass * Connections;
	SOCKET sock;
};
MessageClass *MakeMessage( char * buffer, char * command, char * params [], int paramcount );
