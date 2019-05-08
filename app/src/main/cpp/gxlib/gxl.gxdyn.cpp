#include "gxl.inc.h"
//#include <gx.h>
#include "gapi_help.h"
#include "gxl.gxdyn.h"

//
// only included where its supposed to be

iGAPILoader gGapi;

template< typename T >
inline BOOL 
LoadProc( HMODULE dll, T& fptr, TCHAR* name, bool optional )
{
	fptr = (T)GetProcAddress( dll, name );
	if ( fptr == 0 && !optional ) return FALSE;
	return TRUE;
}

int	fakeGXSetViewport( DWORD top, DWORD height, DWORD, DWORD )
{ return 0; }

BOOL fakeGXIsDisplayDRAMBuffer()
{ return 0; }

int	fakeFuncNoParams()
{ return 0; }


//
//

iGAPILoader::iGAPILoader()
: gxDLL_( 0 )
{
	this->GXOpenDisplay			= 0;
	this->GXCloseDisplay		= 0;
	this->GXBeginDraw			= 0;
	this->GXEndDraw				= 0;
	this->GXSuspend				= 0;
	this->GXResume				= 0;
	this->GXGetDisplayProperties= 0;
	this->GXSetViewport			= 0;
	this->GXIsDisplayDRAMBuffer	= 0;
	this->GXOpenInput			= 0;
	this->GXCloseInput			= 0;
	this->GXGetDefaultKeys		= 0;
}

iGAPILoader::~iGAPILoader()
{
	if ( gxDLL_ ) FreeLibrary( gxDLL_ );
	this->GXCloseInput    = fakeFuncNoParams;
	this->GXCloseDisplay  = fakeFuncNoParams;
}

bool
iGAPILoader::Init( bool forceDefault )
{
	if ( gxDLL_ ) return true;

	TCHAR	name[MAX_PATH];
	_tcscpy( name, _T("gx.dll") );
	
	HMODULE dll = LoadLibrary( name );
	if ( dll ) {
		if ( gxDLL_ ) {
			FreeLibrary( gxDLL_ );
			gxDLL_ = 0;
		}
	
		BOOL ok = TRUE;	
		ok &= LoadProc( dll, this->GXOpenDisplay,		_T("?GXOpenDisplay@@YAHPAUHWND__@@K@Z"), false );
		ok &= LoadProc( dll, this->GXCloseDisplay,		_T("?GXCloseDisplay@@YAHXZ"), false );
		ok &= LoadProc( dll, this->GXBeginDraw,			_T("?GXBeginDraw@@YAPAXXZ"), false );
		ok &= LoadProc( dll, this->GXEndDraw,			_T("?GXEndDraw@@YAHXZ"), false );
		ok &= LoadProc( dll, this->GXSuspend,			_T("?GXSuspend@@YAHXZ"), false );
		ok &= LoadProc( dll, this->GXResume,			_T("?GXResume@@YAHXZ"), false );
		ok &= LoadProc( dll, this->GXGetDisplayProperties,_T("?GXGetDisplayProperties@@YA?AUGXDisplayProperties@@XZ"), false );
		ok &= LoadProc( dll, this->GXSetViewport,		_T("?GXSetViewport@@YAHKKKK@Z"), true );
		ok &= LoadProc( dll, this->GXIsDisplayDRAMBuffer, _T("?GXIsDisplayDRAMBuffer@@YAHXZ"), true );
		ok &= LoadProc( dll, this->GXOpenInput,			_T("?GXOpenInput@@YAHXZ"), false );
		ok &= LoadProc( dll, this->GXCloseInput,		_T("?GXCloseInput@@YAHXZ"), false );
		ok &= LoadProc( dll, this->GXGetDefaultKeys,	_T("?GXGetDefaultKeys@@YA?AUGXKeyList@@H@Z"), false );

		if ( !ok ) {
			FreeLibrary( dll );
			return false;
		}

		gxDLL_ = dll;

		// patch optional fun
		if ( !this->GXSetViewport ) {	
			this->GXSetViewport = fakeGXSetViewport;
		}
		if ( !this->GXIsDisplayDRAMBuffer ) {
			this->GXIsDisplayDRAMBuffer = fakeGXIsDisplayDRAMBuffer;
		}

		return true;
	}

	return false;
}


