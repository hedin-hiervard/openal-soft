#ifndef COMPLIB_XAU_XJP_BLOCK_H_
#define COMPLIB_XAU_XJP_BLOCK_H_
//
// picture jpeg - alike macroblock coding 
//

#include "xbs.cfg.h"
#include "xjp.huff.h"
#include "xjp.dct.h"

namespace xjp {

extern const uint8 ZAG[64];

void encode_block( const dpel_t* pels, dpel_t& last_dc, const huff_etab_t& dcoder, const huff_etab_t& acoder, put_bits_t* pb );
void profile_block( const dpel_t* pels, dpel_t& last_dc, uint32 freq_dc[], uint32 freq_ac[] );
void decode_block( dpel_t* pels, dpel_t& last_dc, const huff_dtab_t& dcoder, const huff_dtab_t& acoder, const dpel_t* qm, get_bits_t* gb );

//void encode_block( const dpel_t* pels, dpel_t pred, dpel_t& last_dc, const huff_etab_t& dcoder, const huff_etab_t& acoder, put_bits_t* pb );
//void profile_block( const dpel_t* pels, dpel_t pred, dpel_t& last_dc, uint32 freq_dc[], uint32 freq_ac[] );
//void decode_block( dpel_t* pels, dpel_t pred, dpel_t& last_dc, const huff_dtab_t& dcoder, const huff_dtab_t& acoder, get_bits_t* gb );

} // xjp


#endif //COMPLIB_XAU_XJP_BLOCK_H_
