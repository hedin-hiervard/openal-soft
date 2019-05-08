//////////////////////////////////////////////////////////////////////////
// MapEnum.cpp
//////////////////////////////////////////////////////////////////////////
// 30.10.2010 created Cotulla
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScopePtr.h"
#include "serialize.h"
#include "Map.h"
#include "MapEnum.h"
#ifdef OS_APPLE
#include <glob.h>
#endif
#include "gxl.inc.h"
#include <vector>
#include <string>
#include "NewMapShop/NewMapShop.h"


// Cotulla: ask Hedin if we need this
/*
bool HasMapVersion(iScenList& scList, iStringT name, GAME_LANG lng) {
for (uint32 xx=0; xx<scList.GetSize(); ++xx)
if(scList[xx]->m_Id == name && cList[xx]->m_lang == lng)
return true;

return false;
}*/


/*void FilterScenarios(iScenList& scList) {
GAME_LANG lng = gSettings.GetLanguage();
iScenList n;
for (uint32 xx=0; xx<scList.GetSize(); ++xx) {          
if(scList[xx]->m_lang == lng || 
(scList[xx]->m_lang == GLNG_ENGLISH && !HasMapVersion(scList, scList[xx]->m_Id, lng))) {
n.Add(scList[xx]);          
}
}
scList = n;
}*/


iMapEnum::iMapEnum()
{

}

iMapEnum::~iMapEnum()
{
    Cleanup();
}



void iMapEnum::Serialize(iFileI* to)
{
    iDynamicBuffer dbuff;
    dbuff.Write((uint16)m_scList.GetSize());
    for (uint32 ssid = 0; ssid < m_scList.GetSize(); ++ssid)
    {
        m_scList[ssid]->Serialize(dbuff);
        ::Serialize(dbuff, m_scList[ssid]->m_FileName);
    }
    to->Write(dbuff.GetData(), dbuff.GetSize());
}

void iMapEnum::Unserialize(iDynamicBuffer& dbuff)
{
    uint16 sscnt;
    dbuff.Read(sscnt);
    if (!sscnt) return;
    m_scList.RemoveAll();
    for (uint16 ssid=0; ssid<sscnt; ++ssid) 
    {
        ScopedPtr<iMapInfo> mapInfo( new iMapInfo() );
        mapInfo->Unserialize(dbuff);
        mapInfo->m_bNewGame = true;
        ::Unserialize(dbuff, mapInfo->m_FileName);
        m_scList.Add(mapInfo.Giveup());
    }
}

const std::vector<std::wstring> free_map_names =
{
    L"avalanche",
    L"break",
    L"territorialdivide",
    L"tutorial"
};

std::wstring base_name(std::wstring const & path)
{
    return path.substr(path.find_last_of(L"/\\") + 1);
}

void iMapEnum::EnumScenarios()
{
    m_scList.RemoveAll();
    
#ifndef OS_APPLE
    iStringT strText = gBundledMapsPath + _T("*.phmd");
    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile(strText.CStr(), &wfd);
    if (hFind != INVALID_HANDLE_VALUE) 
    {
        do 
        {
            if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
            {
                continue;
            }
            //uint64 size = (wfd.nFileSizeHigh * (1+(uint64)MAXDWORD)) + wfd.nFileSizeLow;
            ScopedPtr<iMapInfo> mapInfo( new iMapInfo() );
            mapInfo->m_bNewGame = true;
            mapInfo->m_FileName = gBundledMapsPath + wfd.cFileName;
#ifdef HMM_COMPOVERSION
            iFilePtr pFile(xxc::OpenXFile(mapInfo->m_FileName, HMM_COMPOCODE));
#else           
            iFilePtr pFile(OpenWin32File(mapInfo->m_FileName));
#endif
            if ( !pFile ) continue;
            check(pFile);
            uint32 fourcc; pFile->Read(&fourcc,sizeof(fourcc));             
            if (fourcc == GMAP_FILE_HDR_KEY && mapInfo->ReadMapInfo(pFile.get(), false)) 
            {                   
                m_scList.Add(mapInfo.Giveup());
            }
        } while (FindNextFile(hFind, &wfd));
        FindClose(hFind);
    }
#else
    glob_t g;
    iStringT strText = gDownloadedMapsPath + _T("*.phmd");
    glob(CvtT2A<>(strText.CStr()), GLOB_NOSORT|GLOB_NOESCAPE, NULL, &g);

    strText = gBundledMapsPath + _T("*.phmd");
    glob(CvtT2A<>(strText.CStr()), GLOB_NOSORT|GLOB_NOESCAPE|GLOB_APPEND, NULL, &g);

    for(sint32 x=0; x<g.gl_pathc; x++) {
        ScopedPtr<iMapInfo> mapInfo( new iMapInfo() );
        mapInfo->m_bNewGame = true;
        mapInfo->m_FileName = CvtA2T<>(g.gl_pathv[x]);
        std::wstring basename = base_name(mapInfo->m_FileName.CStr());
        basename = basename.substr(0, basename.length() - 5);
        if(!gMapShop.IsMapPackPurchased() &&
           std::find(free_map_names.begin(), free_map_names.end(), basename) == free_map_names.end())
            continue;
        
        iFilePtr pFile(OpenWin32File(mapInfo->m_FileName));
        
        
        //if ( !pFile ) continue;
        check(pFile);
        uint32 fourcc; pFile->Read(&fourcc,sizeof(fourcc)); 
        if (fourcc == GMAP_FILE_HDR_KEY && mapInfo->ReadMapInfo(pFile.get(), false)) 
        {
            iStringT str = iStringT(mapInfo->m_Id);
            bool flag = false;
            
            for( sint32 y=0; y<m_scList.GetSize(); y++ )
                if( m_scList.At(y)->m_Id == mapInfo->m_Id ){
                    
                    flag = true;
                    break;
                }
            
            if( !flag )
                m_scList.Add(mapInfo.Giveup());
        }   
    }
    globfree(&g);

#endif
      
#ifdef MAP_CACHE
    // save cache
    iFilePtr pFile(CreateWin32File(gMapsPath + _T("maps.cache")));
    if(pFile.get())
    {
        Serialize(pFile.get());
    }
#endif 
	
	// now check against the map stat to fill in the gaps
	for(uint32 xx=0; xx<m_scList.GetSize(); xx++)
	{
		bool bFound = false;
		for(uint32 yy=0; yy<m_stats.GetSize(); yy++)
			if(m_stats[yy].id == m_scList[xx]->m_Name[GLNG_ENGLISH])
			{
				bFound = true;
				break;
			}
		if(!bFound) {
			iMapStatInfo ms;
			ms.id = m_scList[xx]->m_Name[GLNG_ENGLISH];
			ms.played = 0;
            m_stats.Add(ms);
		}
	}
	SaveStat();
}

iMapInfo* iMapEnum::GetScenListItem( uint32 nIndex ) { 
    
    if( nIndex < m_scList.GetSize() ) 
        return m_scList[nIndex]; 

    return NULL;
}

void iMapEnum::Cleanup()
{
    for (uint32 xx = 0; xx < m_scList.GetSize(); ++xx) 
    {
        delete m_scList[xx];
    }
    m_scList.RemoveAll();
}

// called at start up
bool iMapEnum::Init()
{
	// load the map stat file if exists
	iFileStream pStatFile(OpenWin32File(gMapStatFilePath));	
	if (!pStatFile.IsValid()) return true;

	m_stats.RemoveAll();
	uint32 count;
	pStatFile.Read(count);
    
    for(uint32 xx=0; xx<count; xx++) {
        iMapStatInfo ms;
        pStatFile.Read(ms.id);
        pStatFile.Read(ms.played);
        m_stats.Add(ms);
    }
	
    EnumScenarios();
    return true;
}


void iMapEnum::ResetMapStats(const iStringT& id)
{
	for(uint32 xx=0; xx<m_stats.GetSize(); xx++)
	{
		if(m_stats[xx].id == id) {
			m_stats[xx].played = 0;
		}
	}
}

void iMapEnum::MarkMapStarted(const iStringT& id)
{
	bool bAdded = false;
	for(uint32 xx=0; xx<m_stats.GetSize(); xx++)
	{
		if(m_stats[xx].id == id) {
			m_stats[xx].played++;
			bAdded = true;
		}
	}
	
	if(!bAdded) {
		iMapStatInfo ms;
		ms.id = id;
		ms.played = 1;
        m_stats.Add(ms);
	}
	
	SaveStat();
}

void iMapEnum::SaveStat()
{
	// save the stat file
	iFileStream pStatFile(CreateWin32File(gMapStatFilePath));	
	if (!pStatFile.IsValid()) return;

	pStatFile.Write(m_stats.GetSize());
	for(uint32 xx=0; xx < m_stats.GetSize(); xx++)
	{
		pStatFile.Write(m_stats[xx].id);
		pStatFile.Write(m_stats[xx].played);
	}
}

void iMapEnum::Update()
{
    Cleanup();
    EnumScenarios();
}

iScenList iMapEnum::Filter(uint32 mt)
{
    iScenList sl;
    for (uint32 xx = 0; xx < m_scList.GetSize(); ++xx) 
    {
        if (m_scList[xx]->m_globalProperties & mt || m_scList[xx]->NotOfAnyType())
        {
            sl.Add(m_scList[xx]);
        }
    }
    return sl;
}

void iMapEnum::Filter(iScenList &sl, uint32 mask, bool hotseat)
{
    for (uint32 xx = 0; xx < m_scList.GetSize(); ++xx) 
    {
        if (hotseat && m_scList[xx]->HumanPlayers() <= 1)
        {
        	continue;
        }
        if (m_scList[xx]->m_globalProperties & mask || m_scList[xx]->NotOfAnyType())
      
        {
            sl.Add(m_scList[xx]);
        }
    }    
}

uint32 iMapEnum::GetCount(uint32 mask, bool hotseat)
{
    uint32 res = 0;

    for (uint32 xx = 0; xx < m_scList.GetSize(); ++xx) 
    {
        if (hotseat && m_scList[xx]->HumanPlayers() <= 1)
        {
            continue;
        }

        if (m_scList[xx]->m_globalProperties & mask || m_scList[xx]->m_globalProperties == 0)
        {
            res++;
        }
    }    
    return res;
}


uint32 iMapEnum::GetTutorialsNum()
{
    uint32 res = 0;
    for (uint32 xx = 0; xx < m_scList.GetSize(); ++xx) 
    {
        if (m_scList[xx]->m_globalProperties & GMP_GMODE_TUTORIAL)
        {
            res++;
        }
    }  
    return res;
}

uint32 iMapEnum::GetUnplayedNum()
{
	uint32 res = 0;
	for(uint32 xx=0; xx<m_stats.GetSize(); xx++)
		if(m_stats[xx].played == 0)
			res++;
	return res;
}

uint32 iMapEnum::GetPlayed(const iStringT& id)
{
	for(uint32 xx=0; xx<m_stats.GetSize(); xx++)
		if(m_stats[xx].id == id)
			return m_stats[xx].played;
	return 0;
}

// END OF FILE
