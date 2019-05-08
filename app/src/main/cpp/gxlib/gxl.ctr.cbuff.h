#ifndef GXLIB_GXL_CTR_CBUFF_H_
#define GXLIB_GXL_CTR_CBUFF_H_

template <class T, uint32 BUFF_SIZ=64>
class iCircBuff
{
public:
	iCircBuff()
	{
		m_rCur = m_wCur = 0;
		m_aT = new T[BUFF_SIZ];
	}

	~iCircBuff()
	{ 
		delete[] m_aT;
	}

	inline uint32 Count() const
	{ if (m_wCur >= m_rCur) return (m_wCur - m_rCur); else return ((BUFF_SIZ-m_rCur)+m_wCur);}

	void Put(const T& t)
	{
		if ( (m_wCur+1) == m_rCur) {
			check(0);
		}
		m_aT[m_wCur] = t;
		if (++m_wCur == BUFF_SIZ) m_wCur = 0;
	}

	T& Get()
	{
		check(m_rCur != m_wCur);

		uint32 oPos = m_rCur;
		if (++m_rCur == BUFF_SIZ) m_rCur = 0;
		return m_aT[oPos];
	}

	void Reset()
	{
		m_rCur = m_wCur = 0;
	}

private:

	uint32	m_rCur;
	uint32	m_wCur;
	T*		m_aT;
};



#endif //GXLIB_GXL_CTR_CBUFF_H_
