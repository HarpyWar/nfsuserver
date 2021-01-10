#include <winsock2.h>
#include <windows.h>

class ClServerClass {
	public:
		char IP[100];
		unsigned long ip;
		unsigned int Rooms, Users;
		unsigned long TimeOnline;
		bool IsWin32;
		char Name[100];
		bool IsOnline;
		bool IsFav;
		ClServerClass *Next;
		bool Update ();
	char Version[100];
};
class ClServersClass {
	public:
		ClServerClass *First;
		ClServerClass *Last;
		unsigned int Count;
		void AddServer (ClServerClass * Server);
		void RemoveServer (ClServerClass * Server);
		void Save ();
		void Load ();
		void Clear ();
		void Update ();
		void UpdateFromWeb ();
	ClServerClass * ServerFromIP (unsigned long ip);
};
