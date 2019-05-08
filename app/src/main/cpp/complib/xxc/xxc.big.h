#ifndef COMPLIB_XXC_XXC_BIG_H_
#define COMPLIB_XXC_XXC_BIG_H_
//
// Big integers - multiple precision 
//

#include "xxc.params.h"
#include "xxc.fblock.h"

namespace xxc {

//namespace cfg = ::xxc::cfg;

typedef uint16	limb_t;
typedef uint32	wide_t;

// support function
//template< size_t val, size_t align >
//struct static_align_up
//{ enum { result = align * ((val + (align - 1)) / align) };  };

// num elements
static const size_t limb_t_bits = 8 * sizeof(limb_t);
static const size_t limbs_n = ((cfg::M + limb_t_bits - 1)/limb_t_bits + 1);
//static const size_t words_n = 1 + static_align_up<cfg::M + word_t_bits-1, word_t_bits >::result;

// fixed block with words_n + 2 words size, since we need to scratch for overflow and size counter
typedef fblock<limb_t,limbs_n+3> big;

//
void clear( big& v );
void copy( big& d, const big& s );

void word_set( big& v, limb_t w );
void bin_set( big& v, const limb_t* w );

bool equ( const big& a, const big& b );
bool greater( const big& a, const big& b );

size_t numbits( const big& v );
bool bit( const big& v, size_t n );

void add( big& r, const big& a );

void add( big& r, const big& a, const big& b );

void sub( big& r, const big& a );

void sub( big& r, const big& a, const big& b );

void mul( big& p, const big& a, const big& b );

void div( big& r, const big& a, const big& b );
void divrem( big& q, big& u, const big& a, const big& b );

void mod( big& r, const big& a );

void mulmod( big& u, const big& v, const big& w, const big& m );

void modinv( big& r, const big& v, const big& m );

void shl( big& v, size_t n );

void shr( big& v, size_t n );

// actually its xor, but its reserved work
void eor( big& v, const big& x ); 

bool short_mul( big& p, const big& a, limb_t b );

void rand( big& p );


}



#endif //COMPLIB_XXC_XXC_BIG_H_
