#include "stdafx.h"

#include "xxc.cfg.h"
#include "xxc.big.h"

#include "wce.wm5id.h"

namespace xxc {

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))
/* FIXME: This definition of DWORD2BE is little endian specific! */
#define DWORD2BE(x) (((x) >> 24) & 0xff) | (((x) >> 8) & 0xff00) | (((x) << 8) & 0xff0000) | (((x) << 24) & 0xff000000);
/* FIXME: This definition of blk0 is little endian specific! */
#define blk0(i) (block[i] = (rol(block[i],24)&0xFF00FF00)|(rol(block[i],8)&0x00FF00FF))
#define blk1(i) (block[i&15] = rol(block[(i+13)&15]^block[(i+8)&15]^block[(i+2)&15]^block[i&15],1))
#define f1(x,y,z) (z^(x&(y^z)))
#define f2(x,y,z) (x^y^z)
#define f3(x,y,z) ((x&y)|(z&(x|y)))

void wm5id_transform( uint32 state[5], uint8 buffer[64] )
{
	uint32 a, b, c, d, e, t, i, k;
	uint32 *block;

	block = (uint32*)buffer;
	
	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];

	k = 0x5A827999L;
	for (i = 0; i < 16; i++) {
		t = f1(b, c, d) + k + rol(a, 5) + e + blk0(i);
		e = d; d = c; c = rol(b, 30); b = a; a = t;
	}
	for (; i < 20; i++ ) {
		t = f1(b, c, d) + k + rol(a, 5) + e + blk1(i);
		e = d; d = c; c = rol(b, 30); b = a; a = t;
	}
	k ^= ( 0x5A827999L ^ 0x6ED9EBA1L );
	for (; i < 40; i ++) {
		t = f2(b, c, d) + k + rol(a, 5) + e + blk1(i);
		e = d; d = c; c = rol(b, 30); b = a; a = t;
	}
	k ^= ( 0x6ED9EBA1L ^  0x8F1BBCDCL );
	for (; i < 60; i ++) {
		t = f3(b, c, d) + k + rol(a, 5) + e + blk1(i);
		e = d; d = c; c = rol(b, 30); b = a; a = t;
	}
	k ^= ( 0x8F1BBCDCL ^ 0xCA62C1D6L );
	for (; i < 80; i ++) {
		t = f2(b, c, d) + k + rol(a, 5) + e + blk1(i);
		e = d; d = c; c = rol(b, 30); b = a; a = t;
	}

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;

	t = i = a = b = c = d = e = 0;
}



void wm5id_init(wm5id_context* context)
{
   /* SHA1 initialization constants */
	context->state[0] = 0x67452301;
	context->state[1] = 0xEFCDAB89;
	context->state[2] = 0x98BADCFE;
	context->state[3] = 0x10325476;
	context->state[4] = 0xC3D2E1F0;
	context->count[0] =
	context->count[1] = 0;
}

void wm5id_update( wm5id_context* context, const uint8* buf, uint32 size )
{
	uint32 bsize = context->count[1] & 63;
	context->count[1] += size;
	if ( context->count[1] < size ) context->count[0]++;
	context->count[0] += ( size >> 29 );

	if ( bsize + size < 64 ) {
		memcpy( &context->buffer[ bsize ], buf, size );
	} else {
		while ( bsize + size >= 64 ) {
			memcpy( context->buffer + bsize, buf, 64 - bsize );
			buf += 64 - bsize;
			size -= 64 - bsize;
			wm5id_transform( context->state, context->buffer );
			bsize = 0;
		}
		memcpy( context->buffer + bsize, buf, size );
	}
}

void wm5id_final( wm5id_context* context )
{
	int pad;
	uint8 buffer[72];
	uint8* count;
	uint32 bsize, lenhi, lenlo;

	bsize = context->count[1] & 63;
	if ( bsize >= 56 )
		pad = 56 + 64 - bsize;
	else
		pad = 56 - bsize;
	lenhi = (context->count[0] << 3 ) | (context->count[1] >> (32 - 3));
	lenlo = (context->count[1] << 3 );

	memset( buffer + 1, 0, pad - 1 );
	buffer[0] = 0x80;
	count = (uint8*)(buffer + pad);
	count[0] = lenhi >> 24;
	count[1] = lenhi >> 16;
	count[2] = lenhi >> 8;
	count[3] = lenhi;
	count[4] = lenlo >> 24;
	count[5] = lenlo >> 16;
	count[6] = lenlo >> 8;
	count[7] = lenlo;

	wm5id_update( context, buffer, pad + 8 );

	for( uint32 n = 0; n != 5; n++ ) {
		context->state[n] = DWORD2BE( context->state[n] );
	}

//	context->state[0] = 
//	context->state[1] = 
//	context->state[2] = 
//	context->state[3] = 
//	context->state[4] = 0;
	context->count[0] =
	context->count[1] = 0;
}

} //xxc
