#if !defined(LIBRARY_API)
	#define LIBRARY_API /* NOTHING */

	#if defined(WIN32) || defined(WIN64)
		#undef LIBRARY_API
		#if defined(library_EXPORTS)
			#define LIBRARY_API __declspec(dllexport)
		#else
			#define LIBRARY_API __declspec(dllimport)
		#endif
	#endif // defined(WIN32) || defined(WIN64)

#endif // !defined(LIBRARY_API)

