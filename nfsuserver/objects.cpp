#include "objects.h"

extern void Log(char *log);

void MessagesClass::AddMessage( MessageClass * msg ) {
	mut.Lock ();
	msg->Next = NULL;

	if (Count == 0) {
		First = msg;
		Last = msg;
	} else {
		Last->Next = msg;
		Last = msg;
	}

	Count++;
	mut.Unlock ();
} ;

MessageClass *MessagesClass::RemoveFirstMessage() {
	mut.Lock ();

	if (Count > 0) {
		Count--;
		MessageClass * temp = First;
		First = First->Next;

		if (temp == Last) Last = NULL;

		mut.Unlock ();
		return temp;
	} else {
		mut.Unlock ();
		return NULL;
	}
} ;

void MessagesClass::Clear() {
	MessageClass * temp;

	while (Count > 0) {
		temp = RemoveFirstMessage ();
		free (temp->Message);
		free (temp);
	}
} ;

void ConnectionsClass::Clear() {
	ConnectionClass * temp;
	temp = RemoveFirstConnection ();

	while (temp != NULL) {
		temp->IncomingMessages.Clear ();
		temp->OutgoingMessages.Clear ();
		closesocket (temp->sock);
		free (temp);
		temp = RemoveFirstConnection ();
	}
} ;

ConnectionClass *ConnectionsClass::RemoveFirstConnection() {
	mut.Lock ();

	if (Count > 0) {
		Count--;
		ConnectionClass * temp = First;
		First = First->Next;

		if (temp == Last) Last = NULL;
		return temp;
	} else {
		return NULL;
	}

	mut.Unlock ();
} ;

void ConnectionsClass::RemoveConnection( ConnectionClass * con ) {
	if (con == NULL) return;

	if (Count == 0) return;

	mut.Lock ();

	if (Count == 1) {
		First = NULL;
		Last = NULL;
		Count = 0;
	} else {
		if (First == con) {
			First = First->Next;
			Count--;
			mut.Unlock ();
			return;
		}

		ConnectionClass * prev, * temp;
		temp = First;
		prev = NULL;

		while ((temp != con) && (temp != NULL)) {
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

	mut.Unlock ();
} ;

void ConnectionsClass::AddConnection( ConnectionClass * con ) {
	mut.Lock ();
	con->Next = NULL;

	if (Count == 0) {
		First = con;
		Last = con;
	} else {
		Last->Next = con;
		Last = con;
	}

	Count++;
	mut.Unlock ();
} ;

ConnectionsClass::ConnectionsClass() {
	mut.Init ();
	Count = 0;
	First = NULL;
} ;

ConnectionsClass::~ConnectionsClass() {
	Clear ();
	mut.DeInit ();
} ;

bool SendCommand( SOCKET sock, char * command, char * params [], int paramcount ) {
	if (sock == INVALID_SOCKET) return false;

	char buffer[1024];
	int len, k;
	memset (buffer, 0, 12);
	memcpy (buffer, command, strlen (command));
	buffer[11] = 12;
	len = 12;

	for (k = 0; k < paramcount; k++) {
		memcpy (buffer + len, params[k], strlen (params[k]));
		len += (int)strlen (params[k]) + 1;
		buffer[len - 1] = 9;
		buffer[11] = (char)len;
		buffer[10] = (char)(len >> 8);
		buffer[9] = (char)(len >> 16);
		buffer[8] = (char)(len >> 24);
	}

	if (paramcount > 0) {
		buffer[len - 1] = 0;
	}

	k = send (sock, buffer, len, 0);
	return (k > 0);
} ;

int recvn( SOCKET sock, char * buf, int required ) {
	int k = 0;
	int recvsofar = 0;

	while (recvsofar != required) {
		k = recv (sock, buf + recvsofar, required - recvsofar, 0);
		if (k > 0) {
			recvsofar += k;
		} else {
			return recvsofar;
		}
	}

	return recvsofar;
} ;

bool RecvCommand( SOCKET sock, char * buffer ) {
	int k = recvn (sock, buffer, 12);

	if (k == 12) {
		DWORD req = (buffer[11] & 0xFF) | ((buffer[10] << 8) & 0xFF00) | ((buffer[9] << 16) & 0xFF0000)
						| ((buffer[8] << 24) & 0xFF000000);
		req -= 12;

		if (req > 1011) return false;
		if (req > 0) {
			k = recvn (sock, buffer + 12, req);
			if (k != req) k = -1;
		}
	} else {
		k = -1;
	}

	return (k > 0);
} ;

MessageClass *MakeMessage( char * buffer, char * command, char * params [], int paramcount ) {
	int k = MakeCommand (buffer, command, params, paramcount);
	MessageClass * tmsg = (MessageClass *)calloc (1, sizeof (MessageClass));
	tmsg->Message = (char *)calloc (k, sizeof (char));
	memcpy (tmsg->Message, buffer, k);
	tmsg->Size = k;
	return tmsg;
} ;

int MakeCommand( char * buffer, char * command, char * params [], int paramcount ) {
	int len, k;
	memset (buffer, 0, 12);
	memcpy (buffer, command, strlen (command));
	buffer[11] = 12;
	len = 12;

	for (k = 0; k < paramcount; k++) {
		if(params[k]!=NULL){
			memcpy (buffer + len, params[k], strlen (params[k]));
			len += (int)strlen (params[k]) + 1;
			buffer[len - 1] = 9;
			buffer[11] = (char)len;
			buffer[10] = (char)(len >> 8);
			buffer[9] = (char)(len >> 16);
			buffer[8] = (char)(len >> 24);
		}
	}

	if (paramcount > 0) {
		buffer[len - 1] = 0;
	}

	return len;
} ;
