#include "stdafx.h"
#include "common.h"
#include "cm_creatures.h"
#include "SpanMap.h"

iSpannedMap::iSpannedMap(Shape shape, sint32 radius)
{
	check(radius >= 0);
	if (shape == Circle) MakeCircleSpan(radius);
	else if (shape == Square) MakeSquareSpan(radius);
	else {
		check(0);
	}
}

void iSpannedMap::MakeCircleSpan(sint32 radius)
{
	m_SpanList.SetSize(radius*2+1);

	INT cx	= 0;
	INT cy	= radius;
	INT df	= 1 - radius;
	INT d_e	= 3;
	INT d_se= -2 * radius + 5;

	do {	// Filled circle
		m_SpanList[radius-cx] = iSpan(-cx,-cy,cy);
		if ( cx ) m_SpanList[radius+cx] = iSpan(cx,-cy,cy);
		if ( df < 0 ) {
			df	+= d_e;
			d_e	+= 2;
			d_se+= 2;
		} else {
			if( cx != cy ) {
				m_SpanList[radius-cy] = iSpan(-cy,-cx,cx);
				if ( cy ) m_SpanList[radius+cy] = iSpan(cy,-cx,cx);
			}
			df	+= d_se;
			d_e	+= 2;
			d_se+= 4;
			cy--;
		}
		cx++;
	} while ( cx <= cy );
}

void iSpannedMap::MakeSquareSpan(sint32 radius)
{
	sint32 cnt = radius*2+1;
	m_SpanList.SetSize(cnt);

	for (sint32 yy=0; yy<cnt; ++yy) {
		m_SpanList[yy].ypos = (sint16)(-radius + yy);
		m_SpanList[yy].begin = (sint16)-radius;
		m_SpanList[yy].end = (sint16)radius;
	}
}

