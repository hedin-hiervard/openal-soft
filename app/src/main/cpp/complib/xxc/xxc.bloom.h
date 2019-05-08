#ifndef COMPLIB_XXC_XXC_BLOOM_H_
#define COMPLIB_XXC_XXC_BLOOM_H_
// Bloom filter based trapdoor function
// 
// theory:
// capabilities:
// 	- marks arbitrary keys (about %1 of the domain) as valid
//	- produces a predetermined MARK as result (not just binary yes/no)
//  - allows to blacklist specific keys (with modification of key array)
//  - allows to change a MARK without changing a key set
//	- hides a valid preimages with entropy ~ 13bit per valid key
//
// Roughly speaking its a hash function which maps (with current settings)
// 27bit key on 28bit values. 
// only ~10000 keys are returning predefined (valid) value
// function is defined by the table 

#include "xxc.params.h"
#include "xxc.defs.h"
#include "xxc.bswap.h"

namespace xxc {

//
// BLOOM PARAMETERS
#define XXC_BLOOM_PHASES	28		// determines the image size
#define XXC_BLOOM_BITS	    20		// determines array size & keys power
#define XXC_BLOOM_POWER		(1<<XXC_BLOOM_BITS)
#define XXC_BLOOM_SIZE		((1<<XXC_BLOOM_BITS)/8 )
#define XXC_BLOOM_IMASK		((1UL<<XXC_BLOOM_PHASES)-1)

//#define XXC_BLOOM_KEY


//
// bloom hash function
// maps 32 input to 512 bits
void bloom_hash( uint32 word, uint32 img[16] );

//
// bloom retrieve function
// maps N? bit input on Mbit output securely
// TO INLINE OR NOT TO INLINE?
uint32 bloom_probe( const void* buf, uint32 inp );

//
// bloom
uint32 bloom_mix( uint32 a, uint32 c );


} //xxc


#endif //COMPLIB_XXC_XXC_BLOOM_H_
