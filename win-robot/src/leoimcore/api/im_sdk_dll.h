#ifdef WIN32
#ifdef LEOIMCORE_EXPORTS
#define IM_SDK_DLL __declspec(dllexport)
#else
#define IM_SDK_DLL __declspec(dllimport)
#endif
#else
#define IM_SDK_DLL
#endif
