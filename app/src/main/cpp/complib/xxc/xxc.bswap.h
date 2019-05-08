#ifndef COMPLIB_XXC_XXC_BSWAP_H_
#define COMPLIB_XXC_XXC_BSWAP_H_
// byteswapper (originated form xau librayr)
//

#include "xxc.cfg.h"

static always_inline 
uint16 bswap_16( uint16 x )
{
    return (x>>8) | (x<<8);
}

static always_inline 
uint32 bswap_32( uint32 x )
{
  	return ( (x&0xff)<<24) | ((x&0xff00)<<8) | ((x>>8)&0xff00) | ((x>>24)&0xff);
}

// be2me ... BigEndian to MachineEndian
// le2me ... LittleEndian to MachineEndian
#ifdef XXC_CPU_LE
#define be2me_16(x) bswap_16(x)
#define be2me_32(x) bswap_32(x)
#define le2me_16(x) (x)
#define le2me_32(x) (x)
#define me2be_16(x) bswap_16(x)
#define me2be_32(x) bswap_32(x)
#define me2le_16(x) (x)
#define me2le_32(x) (x)
#else
#define be2me_16(x) (x)
#define be2me_32(x) (x)
#define le2me_16(x) bswap_16(x)
#define le2me_32(x) bswap_32(x)
#define me2be_16(x) (x)
#define me2be_32(x) (x)
#define me2le_16(x) bswap_16(x)
#define me2le_32(x) bswap_32(x)


#endif
#ifdef XXC_NO_UNALIGNED_ACCESS
static always_inline uint16 unaligned16( const void* t )
{ return *(const __unaligned uint16*)t; }
static always_inline uint32 unaligned32( const void* t )
{ return *(const __unaligned uint32*)t; }
#else
#    define unaligned16(a) ( *(const uint16*)(a) )
#    define unaligned32(a) ( *(const uint32*)(a) )

#endif
static always_inline uint32 unaligned32_be(const void *v)
{
#ifdef XXC_UNALIGNED_BYTE_READ
        const uint8 *p= (const uint8*)v;
        return (((p[0]<<8) | p[1])<<16) | (p[2]<<8) | (p[3]);
#else
        return be2me_32( unaligned32(v) ); //original
#endif
}

/*
static always_inline uint32 unaligned32_be_( const void* p )
{
	const uint8* bitptr = (const uint8*)p;
	uint32 bits = *bitptr++;		
	bits = (bits << 8) | *bitptr++;		
	bits = (bits << 8) | *bitptr++;		
	bits = (bits << 8) | *bitptr++;		
	return bits;
}
*/

static inline uint32 unaligned32_le(const void *v)
{
#ifdef XXC_UNALIGNED_BYTE_READ
       const uint8 *p= (const uint8*)v;
       return (((p[3]<<8) | p[2])<<16) | (p[1]<<8) | (p[0]);
#else
       return le2me_32( unaligned32(v)); //original
#endif
}


#endif //COMPLIB_XXC_XXC_BSWAP_H_
