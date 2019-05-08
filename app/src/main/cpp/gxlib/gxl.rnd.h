#ifndef GXLIB_GXL_RND_H_
#define GXLIB_GXL_RND_H_

/*
inline DumpF(char* format, ...)
{
    char buff[256];
	va_list argptr;
	int res;

	va_start( argptr, format );
    res = _vsnprintf(buff, 1024, format, argptr);
    va_end(argptr);
	if (res == -1) OutputDebugStringA("== Invalid format ==\n");
	OutputDebugStringA(buff);
	OutputDebugStringA("\n");
}*/


//
class iRandomizer
{
public:
	enum {
		MaxVal = 0x7FFF
	};

public:
	// c-tor
	iRandomizer(uint32 seed = 1)
	{ SetNewSeed(seed); }

	inline uint32 SetNewSeed(uint32 seed) 
	{ 
		m_holdrand = seed;
		//DumpF("---- RND initialization with %d", m_holdrand);
		return m_holdrand;
	}

	inline uint32 GetSeed() const { return m_holdrand; }

	inline sint32 Rand(sint32 maxVal = MaxVal) 
	{ 
		//check(maxVal > 0);
		if(maxVal == 0)
			return 0;
		sint32 res = (sint32)(((m_holdrand = m_holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
		//DumpF("- RND value %d", res);
		if (maxVal != MaxVal) res %= maxVal;
		//DumpF("---- RND returns %d", res);
		return res;
	}

	inline sint32 Rand(sint32 minVal, sint32 maxVal) 
	{ 
		check(maxVal > minVal);
		return minVal + (Rand(maxVal - minVal));
	}

private:
	uint32	m_holdrand;
};


#endif //GXLIB_GXL_RND_H_
