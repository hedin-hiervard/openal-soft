#ifndef PHEROES_GAME_TEXTMANAGER_H_
#define PHEROES_GAME_TEXTMANAGER_H_

//////////////////////////////////////////////////////////////////////////
typedef uint16 TextResId;
extern LPCWSTR *LANG_DATA_ENGLISH;

extern LPCWSTR *LANG_DATA_ENGLISH;
//////////////////////////////////////////////////////////////////////////
class iTextManager
{
public:
	iTextManager();
	~iTextManager();

	bool Init();
	void Cleanup();

    inline const iCharW* English( TextResId resId ) const{
        
        return LANG_DATA_ENGLISH[resId];
    }
    
#if !defined (TRANSLATOR_VERSION)
	inline const iCharW* operator[] (TextResId resId) const
	{
        if(*(LANG_DATA[resId]) != 0)
            return LANG_DATA[resId]; 
        else
            return LANG_DATA_ENGLISH[resId]; 
    }
#else
	inline const iCharW* operator[] (TextResId resId) const
	{ 
		if (m_bHasLngFile){
			check(resId < m_lngData.GetSize()); 
			return m_lngData[resId].CStr();
		} else {
          	check(resId < sizeof(LANG_DATA)/sizeof(iCharW)); 
            return LANG_DATA[resId]; 
		}
	}
	
private:
	bool					m_bHasLngFile;
	iSimpleArray<iStringT>	m_lngData;
#endif
};


#endif //PHEROES_GAME_TEXTMANAGER_H_
