#ifndef COMPLIB_XAU_XAU_PPF_H_
#define COMPLIB_XAU_XAU_PPF_H_
//
// Polyphase filter 

#include "xau.cfg.h"

#define XAU_FILTER_MAX_ORDER 16

struct ppfilter_t {
	sint32	shift;
	sint32	round;
	sint32	error;
	sint32	mode;

	sint32	qm[XAU_FILTER_MAX_ORDER];
	sint32	dx[XAU_FILTER_MAX_ORDER];
	sint32	dl[XAU_FILTER_MAX_ORDER];
};


#endif //COMPLIB_XAU_XAU_PPF_H_
