#ifndef COMPLIB_XAU_XBS_BS_H_
#define COMPLIB_XAU_XBS_BS_H_
//
// bitstream interface (only objects to lower coupling pressure)

#include "xbs.cfg.h"
struct put_bits_t
{
    uint8 *buf;
    uint8 *end;
    uint32 index;
};

struct get_bits_t
{
    const uint8 *buf;
    const uint8 *end;
	uint32 index;
    uint32 bits;
};



#endif //COMPLIB_XAU_XBS_BS_H_
