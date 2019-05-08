#ifndef COMPLIB_XXC_XXC_GF2_H_
#define COMPLIB_XXC_XXC_GF2_H_
//
// Abstract algebra for GF2^m fields
// Galois field of characteristic 2
// over irreducible trinominals
//
// defines: element_t unit_t[units_n]
// unit_t, wide_t

#include "xxc.params.h"
#include "xxc.big.h"

namespace xxc { 
//
// configuration
static const size_t units_n = ((cfg::K+1) * 2);

typedef cfg::unit_t	unit_t;
typedef cfg::wide_t	wunit_t;


//
// type itself
typedef fblock<unit_t,units_n> poly_t;

//
// initialization
void init();
void fini();

//
bool	equ( const poly_t& a, const poly_t& b );

void	clear( poly_t& p );
void	copy( poly_t& d, const poly_t& s );
void	bin_set( poly_t& v, const unit_t* w );

void	rand( poly_t& p );
// r = a + b
void	add( poly_t& r, const poly_t& a, const poly_t& b );
// r = a * b | mod ( x^GF_K + x^GF_T + 1 )
void	mul( poly_t& r, const poly_t& a, const poly_t& b );
// r = (b^(-1))*p | mod ( x^GF_K + x^GF_T + 1 )
void	small_div( poly_t& p, unit_t b );
// r = p^2 | mod ( x^GF_K + x^GF_T + 1 )
void	sqr( poly_t& r, const poly_t& p );
// r = p^(-1) | mod ( x^GF_K + x^GF_T + 1 )
bool	inv( poly_t& r, const poly_t& p );
// r = sqrt(b) = b ^ (2^(GF_M-1))
void	sqrt( poly_t& r, unit_t b );
// evaluates trace of p
size_t	trace( const poly_t& p );
// solves p^2 + p = q
bool	qsolve( poly_t& p, const poly_t& q );
// evaluates rightmost (least-significant) bit of p
bool	ybit( const poly_t& p );
//
void	pack( big& k, const poly_t& p );
void	unpack( poly_t& p, const big& k );

void modulus( big& p );

} // xxc


#endif //COMPLIB_XXC_XXC_GF2_H_
