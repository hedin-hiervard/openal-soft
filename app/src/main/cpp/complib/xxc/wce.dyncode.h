#ifndef COMPLIB_XXC_WCE_DYNCODE_H_
#define COMPLIB_XXC_WCE_DYNCODE_H_
//
// Windows CE ARM - Dynamic Codegen
// Allows to dynamically generate code stub
// to perform secret function


//
// preparing arena for dynamic code depends on VirtualAlloc/etc api
// there is a 3 ways to call these API, ranged by obfuscation level
// 1) Dynamic Linkage - most comatible
// 2) Dynamic Loading Linkage - most compatible
// 3) Export Table HASH dynamic linkage - requre ExpTab compatibility
// 4) Direct call - requre Win32Methods table binary compatibility

#define WCE_CGENARENASIZE	4096

namespace xxc {

// allocates one page for codegen
void*	cgen_alloc();
// prepares arena for writting
void	cgen_lock( void* );
// prepares arena for execution
void	cgen_unlock( void* );
// frees up memory
void	cgen_free( void* );

} //xxc

#endif //COMPLIB_XXC_WCE_DYNCODE_H_
