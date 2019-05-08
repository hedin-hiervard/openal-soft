#ifndef COMPLIB_XAU_XAU_AUDIO_H_
#define COMPLIB_XAU_XAU_AUDIO_H_
//
// Highspeed lossless audio codec v.1
// Based on TTA (True Audio) Copyright 1999-2006 Aleksander Djuric
// 2007-01-09 : SiGMan - functionality separation on low level & middleware

#include "xau.cfg.h"

#include "xbs.bs.h"
#include "xau.rice.h"
#include "xau.ppf.h"

struct xau_codec_t 
{
	put_bits_t	pb;
	get_bits_t	gb;
	rice_ctx_t	rice;
	ppfilter_t	flt;
};

// encode given number of sint8 mono samples to the buffer (which size at least 'samples' bytes)
// returns size of the bytes written into the buffer
size_t xau_pcm8m_encode( const sint8* pcm, size_t samples, void* buf, size_t bufsize );
// decode given number of samples
// returns number of samples decoded
size_t xau_pcm8m_decode( const void* buf, size_t bufsize, sint8* pcm, size_t samples );


#endif //COMPLIB_XAU_XAU_AUDIO_H_
