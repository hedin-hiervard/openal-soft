//
//

#include "stdafx.h"

#include "xxc.cfg.h"
#include "xxc.defs.h"
#include "xxc.app.h"
#include "xxc.gf2.h"
#include "xxc.shash.h"
#include "xxc.dllfn.h"
#include "xxc.security.h"
#include "xxc.sysinfo.h"

#include "xxc.lowlevel.h"
#include "wce.psyscall.h"
#include "wce.wm5id.h"

// external ref to os version
extern xxc::dev_info	gDevInfo;

namespace xxc {

// set the kernel mode
uint32 jump_setkmode( void* str )
{
	// for now use straightforward call

	// OBUSCATED!
	//uint32 cmdBranch = 1;
	//while ( cmdBranch != 0 ) {
	//	switch ( cmdBranch ) {
	//		case 1:
	//			cmdBranch = 2;
	//			break;
	//		case 2:
	//			cmdBranch = 3;
	//			break;
	//		case 3:
	//			cmdBranch = 0;
	//			break;
	//	}
	//}

#ifdef OS_WINCE
	HMODULE hcore = LoadLibrary(L"coredll.dll");
	// store hcore under SEC_HCORE
	snode_t* key = store_insert( str, SEC_HCORE );
	key->data = (void*)hcore;

	BOOL (WINAPI* FuncSetKMode)(BOOL) = NULL;
	FuncSetKMode = (BOOL (WINAPI*)(BOOL)) GetProcAddress ( hcore, L"SetKMode" );



	if (FuncSetKMode) {
		return FuncSetKMode(1);
	}
#endif
	return 0;
}

// ANTIDEBUG: disables low-level fault 
// kernel mode required
uint32 jump_disablefaults( void* )
{
#ifdef OS_WINCE
	XXC_DISABLEFAULTS();
#endif
	return 0;
}

/*
// ANTIDEBUG: detects presence of the debugger
// stores result under SEC_DBG_PRESENT
uint32 jump_checkdebuger( void* str )
{
	// OBUSCATED!
	uint32 cmdBranch = 1;
	uint32 addr = ~0x0000;
	uint32 pptr = 0xdead;

	while ( cmdBranch != 0 ) {
		switch ( cmdBranch ) {
			case 1:
				addr -= 0x37ff;
				// OBFUSCATION
				if ( addr == 0 ) {
					cmdBranch = 1;
				} else {
					addr += 0x090;
					cmdBranch = 2;
				}
				break;
			case 2:
				{
					void* pptr = *((void**)( addr ));
					if ( !pptr ) {
						cmdBranch = 3;
					} else {
						// OBFUSCATION
						cmdBranch = 1;
					}
				}
				break;
			case 3:
				{
					uint32 res = *(uint32*)( (char*)pptr + 0x1c );
					res *= 3;
					// store result
					snode_t* key = store_insert( str, SEC_DBG_PRESENT );
					key->data = (void*)res;
				}
				cmdBranch = 0;
				break;
		}
	}
	return 0;
	//uint32 addrCrypt
	//void* pptr = *((void**)(0xffffc800+0x090));
	//return *(DWORD*)( (char*)pptr + 0x1c );
}

// ANTIDUMP: checks number of PE sections -
// packed file should have exactly 5
// stores result under SEC_PE_SECT
uint32 jump_checkPE( void* str )
{

	// OBFUSCATION
	uint32 z = 11;
	uint32 addr = 0;
	uint32 pptr = 0;
	uint32 res = 71;
	for( uint32 n = 0; n != 100; n++ ) {
		if ( z == 11 ) {
			addr = ~addr;
			addr -= 0x37ff;
			z = 31;
		} else if ( z == 31) {
			addr += 0x090;
			z *= 2;
		} else if ( z == 62 ) {
			void* pptr = *((void**)(addr));
			void* zptr = (void*)( (char*)pptr + 0x50 + 0x10 );
			e32_lite* oe = (e32_lite*)zptr;

			res += oe->e32_objcnt;
			// crypt it as (71 + x)*4  (i.e. correct shoudl be 304 )
			res <<= 2;
			z += res;
		} else {
			// store 
			snode_t* key = store_insert( str, SEC_PE_SECT );
			key->data = (void*)res;
			key++;
			// end of cycle
			z >>= 3;
			n = 100;
		}
	}

	//void* pptr = *((void**)(0xffffc800+0x090));
	//return (void*)( (char*)pptr + 0x50 + 0x10 );
	return 0;
}
*/

// secret HW id - based on direct trap call
uint32 jump_hwid( void* str )
{
	// calc constant
	UINT32 fnAddr = 0xf0000000;
	if ( fnAddr ) {
		fnAddr += 0xffff;
	} else {
		// OBfUSCATION
		fnAddr >>= 4;
	}
	fnAddr -= 0x018B;
	// 0xF000FE74
	// store it under SEC_HWID_FN
	snode_t* key = store_insert( str, SEC_HWID_FN );
	key->data = (void*)KLIO_CE4;//(void*)fnAddr;
	key = 0;

	// create hash object and put its ptr
	xxc::hash hasher;
	hasher.reset();
	key = store_insert( str, SEC_HWID_HASH );
	key->data = (void*)&hasher;

	// retrieve 

	// retrieve addr of check function (SEC_HWID_CHECK1)
	// and call it
	//key = store_find( str, SEC_HWID_CHECK1 );
	
	// calls WM5 function
	uint32 success =  XXC_JUMPID( str, SEC_HWID_CHECK1, str );

	// finalize hash & store result
	uint32 res[8];
	memset( res, 0, sizeof(res) );
	hasher.finalize( res );

	// SEC_HWID_CNST <- result pocket heroes HWID
	key = store_insert( str, SEC_HWID_CNST );
	key->data = (void*)(res[0] + res[1] + res[2]);
	if ( !success ) key->data = (void*)0;

	// disable faults, dummy result
	return XXC_JUMPID( str, SEC_FN_DFAULT, str );
}

uint32 get_ioctl_deviceid()
{
	static volatile uint32 some = 0x01010101;
	uint32 res = some << 15;
	res |= 0x2a;
	res <<= 1;
	check( res == IOCTL_HAL_GET_DEVICEID);
	return res;
}

// kernel func prototype
#ifndef OS_APPLE
typedef BOOL (*kilcfunc)(HANDLE, DWORD, LPVOID, DWORD, LPVOID, DWORD, LPDWORD);
#endif
// data originally used as app_id in GetDe..UniqueId in wm5
BYTE app_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
// returns 1 upon succes
// ---- requires SEC_HWID_FN5 set---
// and SEC_HWID_HASH set

uint32 jump_hwid_check_one( void* str )
{
#ifdef OS_WINCE
       // retrieve hwid
       BYTE hwdata[128];
       DWORD hwsize = sizeof(hwdata);
       wipeout( hwdata, sizeof(hwdata) );
       // TODO: Actually its good idea to hide KLIO_CE4 constant
       kilcfunc  kio_func = (kilcfunc)( store_find( str, SEC_HWID_FN )->data );
       // call the trap
       BOOL res = kio_func( (HANDLE)KMOD_OAL, get_ioctl_deviceid(), NULL, 0,
hwdata, sizeof(hwdata), &hwsize );
       if ( !res ) {
               res = kio_func( (HANDLE)KMOD_OAL, get_ioctl_deviceid(), NULL, 0,
hwdata, sizeof(hwdata), &hwsize );
               if ( !res ) {
                       // bad happen, return 0
                       //wipeout( hwdata, sizeof(hwdata) );
                       return 0;
               }
       }

       // check version, call wm5 wrapper if necessary
       // TODO: place nicely with verison detect
       if ( gDevInfo.version >= 500 ) {
               PDEVICE_ID pdev = (PDEVICE_ID)hwdata;
               wm5id_context ctx;
               wm5id_init( &ctx );
               wm5id_update( &ctx, (uint8*)pdev + pdev->dwPresetIDOffset,
pdev->dwPresetIDBytes );
               if ( pdev->dwPlatformIDBytes )
                       wm5id_update( &ctx, (uint8*)pdev + pdev->dwPlatformIDOffset,
pdev->dwPlatformIDBytes );
               wm5id_update( &ctx, (uint8*)app_data, 8 );
               wm5id_final( &ctx );
               // copy relevant part back
               hwsize = 20;
               memcpy( hwdata, &ctx.state, hwsize );
       }

       // time to apply our hash function
       snode_t* key = store_find( str, SEC_HWID_HASH );
       xxc::hash* h = (xxc::hash*)key->data;
       h->add( hwdata, hwsize );
       wipeout( hwdata, hwsize );

       return 1;
       // call inchain next function
       // return XXC_JUMPID(str, SEC_HWID_CHECK2, str);
#endif
       return 0;
}
uint32 no_jump_hwid_fcall( void* fn, void* buf, uint32 sz )
{
	if ( NULL == fn ) return 0;
	DWORD dwSize = sz;
	DWORD dwCode = get_ioctl_deviceid();
	bool result = false;

	for( uint32 nc = 0; nc != 3; nc++ ) {
		result = !!((fnKIO_t)fn)(dwCode, 0, 0, buf, dwSize, &dwSize);
		if ( result ) break;
		if ( nc > 1 ) dwSize = SIZE_OF_DEVICEID + 16;
	}
	
	if ( result && dwSize > SIZE_OF_DEVICEID ) return dwSize;
	return 0;
}

// should return zero on success
uint32 no_jump_hwid_antihack( void* str )
{
#ifndef OS_APPLE
	// get kioctl pointer
	void* pcore = dll_module( XXC_HV_COREDLL );
	void* pkioc = dll_function( pcore, XXC_HV_KERNELIOCTRL );

	uint32 buf[16];

	uint32 sz = no_jump_hwid_fcall( pkioc, buf, sizeof(buf) );
	
	uint32 cs1 = 0;
	for( uint32* pp = buf; pp < buf+(sz/4); pp++ ) cs1+= *pp;
	buf[0] = cs1;
	buf[1] = (uint32)sz;

	sz = no_jump_hwid_fcall( (void*)SYSCALL_ADDR(SCAPI_KernelIoControl), buf, sizeof(buf) );
	for( uint32* pp = buf; pp < buf+(sz/4); pp++ ) cs1-= *pp;

	return cs1;
#else
	return 0;
#endif
}

//
//


// requires: SEC_HCORE set
uint32 jump_init( void* str )
{
	xxc::init();	
	// prevent debugging (here)

	// get hcore
	snode_t* key = store_find( str, SEC_HCORE );
#ifndef OS_APPLE
	dll_init( (HMODULE)key->data );
#endif

	// init FN ptrs
	key = store_insert( str, SEC_FN_DFAULT );
	key->data = (void*)jump_disablefaults;
//	key = store_insert( str, SEC_FN_CHDBG );
//	key->data = (void*)jump_checkdebuger;
//	key = store_insert( str, SEC_FN_CHPE );
//	key->data = (void*)jump_checkPE;
	key = store_insert( str, SEC_FN_HWID );
	key->data = (void*)jump_hwid;

	// TODO : make somewhere else
	key = store_insert( str, SEC_HWID_CHECK1 );
	key->data = (void*)jump_hwid_check_one;
	//key = store_insert( str, SEC_HWID_CHECK2 );
	//key->data = (void*)jump_hwid_check_two;


	return 0;
}

uint32 jump_done( void* str )
{
#ifndef OS_APPLE
  dll_done();
#endif
	xxc::fini();
	return 1;
}

////////////////////////////////////////////////////////////////////

void* sec_pointers[256];

void sec_initialize()
{
	void* ptr = store_init();
	for( size_t n = 0; n != 256; n++ )
		sec_pointers[n] = ptr;

	// set kmode directly
	jump_setkmode( ptr );

	// init fnptrs
	snode_t* key = store_insert( ptr, SEC_FN_INIT );
	key->data = (void*)jump_init;
	key = store_insert( ptr, SEC_FN_DONE );
	key->data = (void*)jump_done;
}

void sec_finalize()
{
	store_free( sec_pointers[0] );
}

void sec_shuffle()
{
	void* ptr = store_shuffle( sec_pointers[0] );
	for( size_t n = 0; n != 256; n++ )
		sec_pointers[n] = ptr;
}


} //xxc

