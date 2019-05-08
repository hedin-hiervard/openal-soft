#ifndef GXLIB_RES_SFXMANAGER_H_
#define GXLIB_RES_SFXMANAGER_H_

//////////////////////////////////////////////////////////////////////////
class iSfxEntry 
{
public:
	iSfxEntry()
	: m_pBuff(NULL), m_siz(0)
	{ }

	~iSfxEntry()
	{
		if (m_pBuff) {
			free(m_pBuff);
			m_pBuff = NULL;
			m_siz = 0;
		}
	}

	void Allocate(uint32 siz)
	{
		check(m_pBuff == NULL);
		m_pBuff = (sint8*)malloc(siz);
		m_siz = siz;
	}

	uint32 Length() const { return m_siz; }
	const sint8* Data() const { return m_pBuff; }
	sint8* Data() { return m_pBuff; }

private:
	sint8*	m_pBuff;
	uint32	m_siz;
};

//////////////////////////////////////////////////////////////////////////
class iSfxManager
{
public:
	bool Init(const iStringT& fname);
	void Cleanup();
		
	//
	sint32 PlaySound(uint16 idx, bool bLoop = false);
	sint32 ReplaceSound(sint32 channel, uint16 idx);
	static iSfxManager& Instance()
		{
			static iSfxManager theSingleInstance;
			return theSingleInstance;
		}
private:
	/* singleton */
	iSfxManager();
	~iSfxManager();

	typedef iSimpleArray<uint32> iEntryList;

	sint8*		m_pPcmBuff;
	uint32		m_pcmBuffLen;
	void*		m_pXauBuff;
	uint32		m_xauBuffLen;
	iFilePtr	m_pDataFile;
	iEntryList	m_entries;
};


#endif //GXLIB_RES_SFXMANAGER_H_
