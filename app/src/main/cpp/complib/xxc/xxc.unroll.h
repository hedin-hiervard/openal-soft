#ifndef COMPLIB_XXC_XXC_UNROLL_H_
#define COMPLIB_XXC_XXC_UNROLL_H_
//
// Macroses to unroll expressions
//

#ifndef XXC_MACRO_UNROLL_H__
#define XXC_MACRO_UNROLL_H__

#define XXC_CATCAT(x,y) x ## y
#define XXC_CONCAT(x,y) XXC_CATCAT(x,y)

// Extend if necessary
#define XXC_ISEQ_1(m) 					m
#define XXC_ISEQ_2(m) 	XXC_ISEQ_1(m); 	m
#define XXC_ISEQ_3(m) 	XXC_ISEQ_2(m); 	m
#define XXC_ISEQ_4(m) 	XXC_ISEQ_3(m); 	m
#define XXC_ISEQ_5(m) 	XXC_ISEQ_4(m); 	m
#define XXC_ISEQ_6(m) 	XXC_ISEQ_5(m); 	m
#define XXC_ISEQ_7(m) 	XXC_ISEQ_6(m); 	m
#define XXC_ISEQ_8(m) 	XXC_ISEQ_7(m); 	m
#define XXC_ISEQ_9(m) 	XXC_ISEQ_8(m);  m
#define XXC_ISEQ_10(m) 	XXC_ISEQ_9(m);  m
#define XXC_ISEQ_11(m) 	XXC_ISEQ_10(m); m
#define XXC_ISEQ_12(m) 	XXC_ISEQ_11(m); m
#define XXC_ISEQ_13(m) 	XXC_ISEQ_12(m); m
#define XXC_ISEQ_14(m) 	XXC_ISEQ_13(m); m
#define XXC_ISEQ_15(m) 	XXC_ISEQ_14(m); m
#define XXC_ISEQ_16(m) 	XXC_ISEQ_15(m); m

#define XXC_ISEQ_N( n, m ) XXC_CONCAT( XXC_ISEQ_, n )( m )

#define XXC_UNROLL( expr, n ) \
		XXC_ISEQ_N( n, expr ) \
		/* */

//
//

#define XXC_DEFCODE( xxx ) __emit( xxx );

#define XXC_DEFSTUB( start, end, num ) \
	__emit( start );\
	XXC_UNROLL( __emit( 0x12341234 ), num ); \
	__emit( end );\
	/* */

#endif //COMPLIB_XXC_XXC_UNROLL_H_
