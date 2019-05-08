#ifndef GXLIB_GXL_TIMER_H_
#define GXLIB_GXL_TIMER_H_

class iTimer
{
public:
 iTimer() : m_LastTime(GetTickCount()) {}

	bool Init()
	{
		return true;
	}

	inline uint32 GetCurTime()
	{

	  return m_LastTime = GetTickCount();		
	}

	inline uint32 GetStep()
	{
	  uint32 ntime = GetTickCount();	  
	  uint32 sval;
	  if (ntime == m_LastTime) return 0;
	  else if (ntime > m_LastTime) sval = ntime - m_LastTime;
	  else sval = (0xFFFFFFFF - m_LastTime) + ntime;
	  m_LastTime = ntime;
	  return sval;
	}

private:
	uint32	m_LastTime;
};


#endif //GXLIB_GXL_TIMER_H_
