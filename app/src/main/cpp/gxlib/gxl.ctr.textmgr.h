#ifndef GXLIB_GXL_CTR_TEXTMGR_H_
#define GXLIB_GXL_CTR_TEXTMGR_H_


enum TextEntryType {	
    TET_DEFAULT = 0,
    TET_MAP_NAME,       
    TET_MAP_DESC,       
    TET_TEVENT_TEXT,    
    TET_HERO_CNAME,     
    TET_SHIP_CNAME,     
    TET_MAPITEM_MSG,    
    TET_MAPGUARD_INTROMSG,   
    TET_MEVENT_REWMSG,  
    TET_MEVENT_ATTMSG,  
    TET_MEVENT_RFAILMSG, 
    TET_VIS_INTROMSG,        
    TET_CASTLE_CNAME, 
	TET_MAPGUARD_OKREWMSG,
	TET_MAPGUARD_CURFAILMSG,
	TET_MAPGUARD_PERMFAILMSG,
	TET_VIS_OKREWMSG,
	TET_VIS_CURFAILMSG,
	TET_VIS_PERMFAILMSG,
	TET_OWN_INTROMSG,
	TET_OWN_OKREWMSG,
	TET_OWN_CURFAILMSG,
	TET_OWN_PERMFAILMSG,
    TET_COUNT
};




//
class iTextMgr
{
public:
	//
	struct iTextEntry {
		iTextEntry() {}

		iTextEntry(sint16 _idx, const iStringT& _key, TextEntryType _tet)
		: idx(_idx), key(_key), tet(_tet) {}

		sint16		idx;
		sint16		tet;
		iStringT	key;
		iStringT	text[GLNG_COUNT];
	};
	//
	struct KeyValHF {
		uint32 operator() ( const iStringT& str )
		{ return iTables::HashFn( str ); }
		iStringT operator() ( iTextEntry& entry ) 
		{ return entry.key; }
	};

	typedef iHashTable< const iStringT&, iTextEntry, KeyValHF > iKVHash;
	typedef iKVHash::Iterator TextIt;
	typedef iKVHash::ConstIterator ConstTextIt;

private:
	iKVHash			m_TextHash;
	sint16			m_idxCounter;


public:

	inline iTextEntry* GetUnsafeEntryPtr(const iStringT& key)
	{
		iKVHash::Iterator it = m_TextHash.Find(key);
		if (it==m_TextHash.End()) return NULL;
		return &(*it);
	}

	void Reset()
	{
		m_TextHash.RemoveAll();
		m_idxCounter = 0;
	}


	iTextEntry& AddTextEntry(const iStringT& _key, TextEntryType _tet = TET_DEFAULT)
	{
/*		if ( m_TextHash.Find(_key) != m_TextHash.End()) {
			MessageBox(0, _key.CStr(), _T("Redefined text"), MB_OK );
		}
*/		
		iTextEntry& nentry = *m_TextHash.Insert(iTextEntry(m_idxCounter++, _key, _tet));
		return nentry;
	}

	iTextEntry& AddTextEntry(const iStringT& _key, const iStringT& _orig, TextEntryType _tet = TET_DEFAULT)
	{
		//check(m_TextHash.Find(_key) == m_TextHash.End());
		/*if ( m_TextHash.Find(_key) != m_TextHash.End()) {
			MessageBox(0, _key.CStr(), _T("Redefined text"), MB_OK );
		}*/
		iTextEntry& nentry = *m_TextHash.Insert(iTextEntry(m_idxCounter++, _key, _tet));
		nentry.text[GLNG_ENGLISH] = _orig;
		return nentry;
	}

	inline const iTextEntry& GetEntry(const iStringT& key) const
	{
		iKVHash::ConstIterator it = m_TextHash.Find(key);
		check (it!=m_TextHash.End());
		return (*it);
	}

	inline const iStringT& operator[] (const iStringT& key) const
	{
		return GetEntry(key).text[GLNG_ENGLISH];
	}

	inline bool HasEntry(const iStringT& key) const { return m_TextHash.Find(key) != m_TextHash.End(); }

	inline const iKVHash& GetHash() const { return m_TextHash; }
	inline iKVHash& GetUnsafeHash() { return m_TextHash; }
	
	inline void RemoveEntry(const iStringT& key)
	{
		iKVHash::Iterator it = m_TextHash.Find(key);
		check (it!=m_TextHash.End());
		m_TextHash.Remove(it);
	}

	inline iStringT& GetText(const iStringT& key, GAME_LANG lng) 
	{
		iKVHash::Iterator it = m_TextHash.Find(key);
		if((it!=m_TextHash.End()))
			return (*it).text[lng];
		//else throw iFormat(_T("No text entry found for %s"), key.CStr());
	}

};
//
bool AddLanguageFile(const iStringT& fileName, GAME_LANG lang, iTextMgr& tmgr, bool bPrimary);

#endif //GXLIB_GXL_CTR_TEXTMGR_H_
