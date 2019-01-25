#ifdef WIN32
	#ifdef LEONETWORK_EXPORTS
		#define NETWORK_DLL __declspec(dllexport)
	#else
		#define NETWORK_DLL __declspec(dllimport)
	#endif
#else
		#define NETWORK_DLL
#endif
