#ifndef GXLIB_GXL_DEBUG_H_
#define GXLIB_GXL_DEBUG_H_

#ifdef OS_WIN32

template<uint32 cnt>
class iPerfTimer
{
public:
	iPerfTimer()
	{ 
		for (uint32 xx=0; xx<cnt; ++xx){
			m_TimeAnchor[xx] = -1;
			m_TimeVal[xx] = 0;
		}
	}

	void Begin(uint32 tid)
	{
		check(tid < cnt);
		check(m_TimeAnchor[tid] == -1);
		m_TimeAnchor[tid] = GetTickCount();
	}

	uint32 End(uint32 tid)
	{
		check(tid < cnt);
		check(m_TimeAnchor[tid] != -1);
		uint32 per = GetTickCount() - m_TimeAnchor[tid];
		m_TimeVal[tid] += per;
		m_TimeAnchor[tid] = -1;
		return per;
	}

	float GetTotalTime()
	{
		uint32 res = 0;
		for (uint32 xx=0; xx<cnt; ++xx){
			res += m_TimeVal[xx];
		}
		return (float)(res)/1000;
	}

	void DumpResult()
	{
		WCHAR buff[1024]={0};
		float total_time = GetTotalTime();
		wsprintf(buff,L"Perfomance Counter Result Dump:\n");
		wsprintf(buff,L"-------------------------------\n");
		if (total_time > 0){
			for (uint32 xx=0; xx<cnt; ++xx){
				WCHAR sbuff[64]={0};
				float stime = (float)(m_TimeVal[xx])/1000.0f;
				wsprintf(sbuff,L"Timer #03d:  %.2f sec  %.2f %%",xx+1,stime,stime*100/total_time);
				wcscat(buff,sbuff);
			}
		}
		wsprintf(buff,L"-------------------------------\n");
		wsprintf(buff,L"TOTAL: %.2f sec\n",total_time);
		OutputDebugString(buff);
	}

private:
	uint32	m_TimeAnchor[cnt];
	uint32	m_TimeVal[cnt];
};


#endif //OS_APPLE

#endif //GXLIB_GXL_DEBUG_H_
