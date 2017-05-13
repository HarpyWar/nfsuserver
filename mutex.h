#ifdef _WIN32

class Mutex {
	private:
		CRITICAL_SECTION lock;

	public:
		void Init( void ) {
			InitializeCriticalSection (&lock);
		}

		void DeInit( void ) {
			DeleteCriticalSection (&lock);
		}

		void Lock( void ) {
			EnterCriticalSection (&lock);
		}

		void Unlock( void ) {
			LeaveCriticalSection (&lock);
		}
};
#else

class Mutex {
	private:
		pthread_mutex_t lock;
		pthread_mutexattr_t mattr;

	public:
		void Init( void ) {
			pthread_mutexattr_init (&mattr);
//			pthread_mutexattr_setpshared (&mattr, PTHREAD_PROCESS_SHARED);
			pthread_mutex_init (&lock, &mattr);
		}

		void DeInit( void ) {
			pthread_mutex_destroy (&lock);
			pthread_mutexattr_destroy (&mattr);
		}

		void Lock( void ) {
			pthread_mutex_lock (&lock);
		}

		void Unlock( void ) {
			pthread_mutex_unlock (&lock);
		}
};
#endif
