#ifndef GXLIB_GXL_CTR_SPANMAP_H_
#define GXLIB_GXL_CTR_SPANMAP_H_

class iSpannedMap 
{
public:
	enum Shape {
		Circle = 0,
		Square
	};

	struct iSpan {
		iSpan(sint16 _ypos = 0, sint16 _begin = 0, sint16 _end = 0)
		: ypos(_ypos),begin(_begin), end(_end){}

		sint16	ypos;
		sint16	begin;
		sint16	end;
	};

	// Constructs with circle or square spaned map
	iSpannedMap(Shape shape = Square, sint32 radius = 1);

	// D-tor
	~iSpannedMap(){}

	// inlines
	inline uint32 SpanLinesCount() const
	{ return m_SpanList.GetSize(); }

	inline const iSpan& operator[](sint32 idx) const
	{ check(idx>=0 && idx<(sint32)m_SpanList.GetSize()); return m_SpanList[idx]; }

private:
	void MakeCircleSpan(sint32 radius);
	void MakeSquareSpan(sint32 radius);

	iSimpleArray<iSpan>	m_SpanList;
};


#endif //GXLIB_GXL_CTR_SPANMAP_H_
