#ifndef COMPLIB_XAU_XJP_DCT_H_
#define COMPLIB_XAU_XJP_DCT_H_
//
// basic dct / idct transfroms
// credits goes to Pascal Massimino
// based on "Fast and precise" LLM implementation of FDCT/IDCT paper
// 2007-12-07 : SiGMan - initial revision

#include "xbs.cfg.h"

namespace xjp {

typedef sint16 dpel_t;

// inplace fdct transformation
void fdct16( dpel_t* val );
// inplace idct transformation
void idct16( dpel_t* val );


} //xjp


#endif //COMPLIB_XAU_XJP_DCT_H_
