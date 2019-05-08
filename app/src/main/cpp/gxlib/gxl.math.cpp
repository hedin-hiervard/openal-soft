#include "gxl.inc.h"

#ifndef OS_APPLE
#include <windows.H>
#endif

#include <MATH.H>
//#include "gxl.common.def.h"
//#include "gxl.fixpoint.h"
#include "gxl.math.h"

float SIN_TABLE_FLT[256];
float COS_TABLE_FLT[256];
fix32 SIN_TABLE_FIX[256];
fix32 COS_TABLE_FIX[256];

void InitMath()
{
	float pi2 = PI*2.0f;
	float pi05 = PI/2.0f;
	for (uint32 xx=0; xx<256; ++xx){
		float fxx = (float)xx;
		float sval = (float)sin((fxx/256.0f*pi2) - pi05);
		float cval = (float)cos((fxx/256.0f*pi2) - pi05);

		SIN_TABLE_FLT[xx] = sval;
		COS_TABLE_FLT[xx] = cval;
		SIN_TABLE_FIX[xx] = sval;
		COS_TABLE_FIX[xx] = cval;
	}
}
