#ifndef COMPLIB_XXC_XXC_SDH_H_
#define COMPLIB_XXC_XXC_SDH_H_
//
// Simplified (non-secure) Diffie-Hellman key exchange
//

#include "xxc.defs.h"
#include "xxc.bonus.h"

namespace xxc {

// remainder of division fn 
uint32 div_urem(uint32 dividend, uint32 divisor );

// exp by squaring (used inline)
always_inline
uint32 exp_mod( uint32 n, uint32 p, uint32 m )
{
	uint32 res = 1;
	while ( p != 0 ) {
		if ( p & 1 ) {
			//res = (res * n) % m;
			res = div_urem( res * n, m );
			p--;
		}
		//n = (n*n) % m;
		n = div_urem( n * n, m );
		p /= 2;
	}
	return res;
}

// module and generator constants
const uint32 sdh_mod	= 64019;
const uint32 sdh_gen	= 2007;

/*
 Algorithm to use:
 - generate random 'a'
 - calc A and pass to function
 - - generate random 'b'
 - - calc Kb (A,b)
 - - calc B and pass back
 - calc Ka (B,a) and store
 - Ka == Kb
 - Ka, Kab <> [0..sdh_mod)
 */

/*
// G( q, g )
// Diffie-Hellman key exchange
void ensure( uint a, uint b )
{
	// a & b are secret
	// group
	// 33614 / 1437
	uint p = 64019;
	uint g = 2007; //638;
	uint A = pmod_sq( g, a, p );
	// >> (g, p), A
	uint B = pmod_sq( g, b, p );
	// << B
	uint Kb= pmod_sq( A, b, p );
	uint Ka= pmod_sq( B, a, p );
	printf("Ka:%d Kb:%d\n", Ka, Kb );
}
*/

} //xxc


#endif //COMPLIB_XXC_XXC_SDH_H_
