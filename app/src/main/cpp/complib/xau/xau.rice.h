#ifndef COMPLIB_XAU_XAU_RICE_H_
#define COMPLIB_XAU_XAU_RICE_H_
//
// Two 'K' skewed rice coder. 
// Has decent performance on distributions close to laplacian 
// 2007.12.20 : SiGMan - bs refactored to separate source
// 
#include "xau.cfg.h"
#include "xbs.bs.h"

struct rice_param_t
{
	uint32	k;
	uint32	sum;
};

typedef rice_param_t rice_ctx_t[2];


#endif //COMPLIB_XAU_XAU_RICE_H_
