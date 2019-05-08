#ifndef COMPLIB_XXC_XXC_DEFS_H_
#define COMPLIB_XXC_XXC_DEFS_H_
// portability macroses
//

#include "xxc.cfg.h"

always_inline uint32 rotate_left32( uint32 v, size_t n )
{ 
	return n ? uint32(v << n) | (v >> (32 - (n))) : v;
}

always_inline uint32 rotate_right32( uint32 v, size_t n )
{ 
	return n ? uint32(v << (32 - n)) | (v >> n) : v;
}

always_inline void wipeout( void* p, size_t bytes )
{
	memset( p, 0, bytes );
}

always_inline void copybytes( void* to, const void* from, size_t bytes )
{
	memcpy( to, from, bytes );
}


#define XXC_ROTL32C( v, n )	rotate_left32( v, n )
#define	XXC_ROTR32C( v, n ) rotate_right32( v, n )

#define XXC_ROTL32( v, n ) XXC_ROTL32C( v, n )
#define XXC_ROTR32( v, n ) XXC_ROTR32C( v, n )

#define XXC_LOAD32( ptr )
#define XXC_STORE32( ptr, val )

#endif //COMPLIB_XXC_XXC_DEFS_H_
