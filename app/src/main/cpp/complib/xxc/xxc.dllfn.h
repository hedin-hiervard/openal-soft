#ifndef COMPLIB_XXC_XXC_DLLFN_H_
#define COMPLIB_XXC_XXC_DLLFN_H_
//
// 6ae38d4d 487066bd 2f8abb18

#include "xxc.cfg.h"
//
// Name hash values 
#define XXC_HV_COREDLL		0x6ae38d4d
#define XXC_HV_KERNELIOCTRL 0x487066bd
#define XXC_HV_GETUNIQUEID	0x2f8abb18
#define XXC_HV_GETFILEATTEX	0xf9ba6d32
#define XXC_HV_GETMODULEINF	0xcc43d203
#define XXC_HV_VIRTPROTECT	0x1000c04c

namespace xxc {

// initialize module
// should be called far from the usage point
// i.e. at the program start
#ifndef OS_APPLE
bool  dll_init( HMODULE hCoreDLL );
#endif
void  dll_done();

// find module handle by name hash
// returns NULL if fails
void* dll_module( uint32 hash );
// finds function by module handle and name hash
// returns NULL if fails
void* dll_function( void* module, uint32 hash );
void* dll_funclist( void* module, uint32* hashes, void** func );

} // xxc


#endif //COMPLIB_XXC_XXC_DLLFN_H_
