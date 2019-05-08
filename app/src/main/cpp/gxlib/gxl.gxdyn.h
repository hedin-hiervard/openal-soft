#ifndef GXLIB_GXL_GXDYN_H_
#define GXLIB_GXL_GXDYN_H_

#define GXProps GXDisplayProperties

struct iGAPILoader
{
	HMODULE	gxDLL_;

	int		(*GXOpenDisplay)(HWND hWnd, DWORD dwFlags );
	int		(*GXCloseDisplay)();
	void*	(*GXBeginDraw)();
	int		(*GXEndDraw)();
	int		(*GXSuspend)();
	int		(*GXResume)();
	GXDisplayProperties	(*GXGetDisplayProperties)();
	int		(*GXSetViewport)( DWORD top, DWORD height, DWORD, DWORD );
	BOOL	(*GXIsDisplayDRAMBuffer)();
	int		(*GXOpenInput)();
	int		(*GXCloseInput)();
	GXKeyList (*GXGetDefaultKeys)(int);

	iGAPILoader();
	~iGAPILoader();

	bool Init( bool forceDefault );

	bool IsInitialized() const
	{ return gxDLL_ != 0; }
};

extern iGAPILoader gGapi;



#endif //GXLIB_GXL_GXDYN_H_
