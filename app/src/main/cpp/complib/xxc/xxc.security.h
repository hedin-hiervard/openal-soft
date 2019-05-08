#ifndef COMPLIB_XXC_XXC_SECURITY_H_
#define COMPLIB_XXC_XXC_SECURITY_H_
//
// logical jumper functions and such
// most app security is hidden here
//

#include "xxc.defs.h"
#include "xxc.store.h"

namespace xxc {


// constants
#define SEC_FN_KMODE	0x439122
#define SEC_FN_INIT		0x123
#define SEC_FN_DONE		0x124

#define SEC_FN_DFAULT	0x59191
#define SEC_FN_CHDBG	0x99952
#define SEC_FN_CHPE		0x592135
#define SEC_FN_HWID		0x3321

#define SEC_DBG_PRESENT	0x939824
#define SEC_PE_SECT		0x8776a31
#define SEC_HWID_FN		0x1ad923
#define SEC_HWID_CHECK	0x942
#define SEC_HWID_CHECK1	0x94233
#define SEC_HWID_CHECK2	0x45942
#define SEC_HWID_HASH	0x93921
#define SEC_HWID_CNST	0x8311111
#define SEC_HCORE		0xab38114

#define SEC_OEMHASH		0x821
#define SEC_REGISTERED	0x72932

#define SEC_REG_VALUE	0x00012
#ifndef OS_APPLE
#define IS_REGISTERED() (xxc::store_find( XXC_GET_SEC(), SEC_REGISTERED )->data == (void*)SEC_REG_VALUE);
#define SET_REGISTERED() xxc::store_insert( XXC_GET_SEC(), SEC_REGISTERED )->data = (void*)SEC_REG_VALUE;
#else
#define IS_REGISTERED() true
#define SET_REGISTERED()
#endif
#define	SEC_BLOOMHASH	0x0a0d

// same values - pick them at random
// contermeasure against HW bkpt on memory location
extern void* sec_pointers[256];

// random picker
#define XXC_GET_SEC() (xxc::sec_pointers[ ::rand() & 0xff])

// jumper function prototype
typedef uint32 (*jumper_t)( void* );
// jumper macro
#define XXC_JUMP( fnp, arg )	((jumper_t)fnp)( (void*)(arg) ) 
#define XXC_JUMPID( stg, id, arg ) ( (jumper_t)(store_find( stg, id )->data))( (void*)(arg) )
#define XXC_JUMPID_( stg, id, arg ) ( (xxc::jumper_t)(xxc::store_find( stg, id )->data))( (void*)(arg) )

// initializer
void sec_initialize();
void sec_finalize();
void sec_shuffle();


// entry point #1
// initialization
uint32 jump_init( void* );
uint32 jump_done( void* );

// set kernel mode
uint32 jump_setkmode( void* );

// disable faults 
uint32 jump_disablefaults( void* );

// debugger check
// > SEC_DBG_PRESET != 0
uint32 jump_checkdebuger( void* );

// check sections number
// > SEC_PE_COUNT == 304
uint32 jump_checkPE( void* );

// secret HW id
// >SEC_HW_ID
uint32 jump_hwid( void* );
uint32 jump_hwid_check_wm5( void* );
uint32 jump_hwid_check_kio( void* );

// check if KICTL is not intercepted
// returns 0 if its same with syscall
uint32 no_jump_hwid_antihack( void* );



} //xxc


#endif //COMPLIB_XXC_XXC_SECURITY_H_
