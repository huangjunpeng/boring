#ifndef LIBXL_SETUP_C_H
#define LIBXL_SETUP_C_H


#ifdef WIN32
	#ifdef UPLOADSTATUSEX_EXPORTS
		#define SOUTHAPI __declspec(dllexport)
	#else
		#define SOUTHAPI __declspec(dllimport)
	#endif
#endif

#endif //LIBXL_SETUP_C_H