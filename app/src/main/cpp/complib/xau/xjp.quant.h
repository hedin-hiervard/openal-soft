#ifndef COMPLIB_XAU_XJP_QUANT_H_
#define COMPLIB_XAU_XJP_QUANT_H_
//
// 2007-12-10 : SiGMan - simplified quantizer interface

#include "xbs.cfg.h"

namespace xjp {

// prepare quantizer table
void quant_init( sint16 quant[64], uint32 tabn, sint32 param, const uint8* order );

// forward quantization step
static always_inline
sint16 quantize( sint32 pel, sint16 qval )
{
//	return (2*pel+1) / (2*qval);
	return ((pel<<4) + (pel>0?qval:-qval)) / (2*qval);
//	return (pel + 1)/2;
//	return ((pel<<4) / (2*qval));
//	return (pel-(qval>>1))/(qval<<1);
}

// backward quantization stemp
static always_inline
sint16 dequantize( sint32 pel, sint16 qval )
{
//	return pel * qval;
	return ((2 * pel * qval) >> 4);
//	return pel*2;
//	return (((2 * pel + 1) * qval) >> 4);
//	return ( pel*(qval<<1) + ((qval&1)? qval : qval-1) );
}


} // xjp


#endif //COMPLIB_XAU_XJP_QUANT_H_
