//
//


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "wce.dyncode.h"

namespace xxc {

void*	cgen_alloc()
{
	return VirtualAlloc( NULL, 1024, MEM_COMMIT, /*PAGE_EXECUTE_READ |*/ PAGE_EXECUTE_READWRITE );
}

void	cgen_lock( void* p )
{
	DWORD prot;
	VirtualProtect( p, 1024, PAGE_READWRITE | PAGE_NOCACHE, &prot );
}

void	cgen_unlock( void* p )
{
	DWORD prot;
	VirtualProtect( p, 1024, PAGE_EXECUTE_READ, &prot );
}

void	cgen_free( void* p )
{
	VirtualFree( p, 0, MEM_RELEASE );
}


} //xxc
