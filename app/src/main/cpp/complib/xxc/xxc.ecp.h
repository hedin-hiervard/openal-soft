#ifndef COMPLIB_XXC_XXC_ECP_H_
#define COMPLIB_XXC_XXC_ECP_H_
//
// Elliptic curve math on GF2
//

#include "xxc.params.h"
#include "xxc.big.h"
#include "xxc.gf2.h"

namespace xxc {

typedef big	big_t;

//
struct point_t
{
	poly_t x;
	poly_t y;
};

////////////////////////////////////////////////////////

// confirm y2 + xy = x3 + B for p
bool validate( const point_t& p );

//
bool equ( const point_t& a, const point_t& b );

// 
void copy( point_t& d, const point_t& s );

// wipe out 
void clear( point_t& p );

// given x coord of the p finds out y
bool calc_y( point_t& p, bool ybit );

//
void add( point_t& p, const point_t& a );

//
void sub( point_t& p, const point_t& a );

// -p
void neg( point_t& p );

// 2*p
void dbl( point_t& p );

//
void mul( point_t& p, const big& a );

// 0 if 'x' == 0 or to (y/x) otherwise
bool ybit( const point_t& p );

void pack( big& i, const point_t& p );
void unpack( point_t&, const big& );


} // xxc


#endif //COMPLIB_XXC_XXC_ECP_H_
