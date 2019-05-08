#ifndef RESLIB_AVATARMGR_H_
#define RESLIB_AVATARMGR_H_

/*
 *	
 */
class iAvatarMgr
{
	//
	struct KeyVal {
		KeyVal() : m_Value(_T(""),_T(""),0) {}
		inline KeyVal( const iStringT& k) : m_Key( k ), m_Value(k , _T(""),0) {}

		iStringT	m_Key;
		iAvatar		m_Value;
	};

	//
	struct KeyValHF {
		uint32 operator() ( const iStringT& str )
		{ return iTables::HashFn( str ); }
		iStringT operator() ( KeyVal& pair ) 
		{ return pair.m_Key; }
	};

	uint32	m_Counter;

public:
	iAvatarMgr() : m_Counter(0) {}

	typedef iHashTable< const iStringT&, KeyVal, KeyValHF > iKVHash;
	typedef iKVHash::Iterator		AvatarIt;
	typedef iKVHash::ConstIterator	ConstAvatarIt;
	iKVHash		m_AvatarHash;

	iAvatar* AddAvatar(const iStringT& id, const iStringT& nameKey)
	{
		AvatarIt it = m_AvatarHash.Insert(id);
		(*it).m_Value.SetIdx(m_Counter++);
		(*it).m_Value.SetId(id);
		(*it).m_Value.SetNameKey(nameKey);
		return &(*it).m_Value;
	}

	const iAvatar&  operator[](const iStringT& id) const
	{
		ConstAvatarIt it = m_AvatarHash.Find(id);
		if (it==m_AvatarHash.End()) {
			check(0);
		}
		const iAvatar& result = (*it).m_Value;
		return result;
	}
};

#endif //RESLIB_AVATARMGR_H_
