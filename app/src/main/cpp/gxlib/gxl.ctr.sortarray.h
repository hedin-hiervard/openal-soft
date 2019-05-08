#ifndef GXLIB_GXL_CTR_SORTARRAY_H_
#define GXLIB_GXL_CTR_SORTARRAY_H_

//
// WARN!! Must be refactored according to the SimpleArray changes
// NOTE:: Provide a way to traits-qualify the internal iEntry structure
//		according to member T

//#ifdef XXL_DECLARE_AS_POD_IF_POD
//XXL_DECLARE_AS_POD_IF_POD( )

//#endif 
template <class T>
class iSortArray
{
public:
	struct iEntry {
		iEntry() : idx(0), T(0) {}
		iEntry(intptr_t _idx, const T& _value) : idx(_idx), value(_value) {}
		intptr_t	idx;
		T		value;
	};
public:

	inline iEntry* GetPtr()
	{ return m_Array.GetData(); }

	inline const iEntry* GetPtr() const
	{ return m_Array.GetData(); }

	void Init(const iSortArray<T>& other)
	{
		iSortArray<T>& me = *this;
		me = other;
	}

	inline T Pop()
	{
		return m_Array.Pop().value;
	}

	inline sint32 FirstIdx() const { return m_Array[0].idx; }
	inline sint32 LastIdx() const { return m_Array.GetLast().idx; }

	inline T& Last() { return m_Array.GetLast().value; }
	inline const T& Last() const { return m_Array.GetLast().value; }

	bool RemoveAt(uint32 idx)
	{ return m_Array.RemoveAt(idx); }

	// indexes: from smaller to bigger 
	bool Insert(const T& value, intptr_t idx)
	{
		uint32 length = m_Array.GetSize();
		if (!length || m_Array[length-1].idx <= idx) {
			m_Array.Add(iEntry(idx, value));
			return true;
		}

		if (idx < m_Array[0].idx) {
			m_Array.InsertBefore(0,iEntry(idx, value));
			return true;
		}

		uint32 middle, half;
		uint32 first = 0;

		while (length > 0) {
			half = length >> 1;
			middle = first + half;
			if ( m_Array[middle].idx <= idx ) {
				first = middle+1;
				length = length - half - 1;
			} else {
				length = half;
			}
		}
		m_Array.InsertBefore(first,iEntry(idx, value));
		return true;
	}

	void Cleanup() { m_Array.RemoveAll(); }

	inline uint32 Size() const { return m_Array.GetSize(); }

	inline const iEntry& operator[] (uint32 idx) const { return m_Array[idx]; }
	inline const T& Value(uint32 idx) const { return m_Array[idx].value; }
	inline T& Value(uint32 idx) { return m_Array[idx].value; }
	inline sint32 Index(uint32 idx) const { return m_Array[idx].idx; }

	inline bool SelfTest() const
	{
		const uint32 siz = m_Array.GetSize();
		if ( siz < 2 ) return true;
		
		for( uint32 n = 0; n != siz - 1; ++n ) {
			if ( m_Array[n].idx > m_Array[n+1].idx ) return false;
		}

		return true;
	}

protected:
	iSimpleArray<iEntry>	m_Array;
};


#endif //GXLIB_GXL_CTR_SORTARRAY_H_
