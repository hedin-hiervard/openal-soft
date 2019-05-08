#include "stdafx.h"
#include "Dlg_ScenList.h"
#include "Ctl_ScenList.h"


//////////////////////////////////////////////////////////////////////////
iScenListDlg::iScenListDlg(iViewMgr* pViewMgr, uint32 filter)
: iBaseGameDlg(pViewMgr), m_filter(filter)
{}

iScenListDlg::~iScenListDlg()
{}

void iScenListDlg::OnCreateDlg()
{
    iRect clRect = ClientRect();
    AddChild(m_ctrl = new iScenListView(m_pMgr, this, clRect, 109, m_filter));    
}


iSize iScenListDlg::ClientSize() const
{ 
    return gApp.Surface().GetSize();
}


void iScenListDlg::DoCompose(const iRect& rect)
{
}


const iMapInfo& iScenListDlg::SelScen() const
{
    return m_ctrl->SelScen();    
}


void iScenListDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
    uint32 uid = pView->GetUID();
    if (uid == 109)
    {
        // End dialog
        EndDialog(param); 
    }
}




#if 0
#include "ScopePtr.h"
#include "Dlg_ScenList.h"
#include "xxc/xxc.file.h"
#include "serialize.h"
#ifdef OS_IPHONE
#include <glob.h>
#endif
#include "gxl.inc.h"
/*
 *	
 */
inline void Serialize(iFileI* to, const iScenList& scList)
{
	iDynamicBuffer dbuff;
	dbuff.Write((uint16)scList.GetSize());
	for (uint32 ssid=0; ssid<scList.GetSize(); ++ssid) {
		scList[ssid]->Serialize(dbuff);
		::Serialize(dbuff, scList[ssid]->m_FileName);
	}
	to->Write(dbuff.GetData(), dbuff.GetSize());
}

inline void Unserialize(iDynamicBuffer& dbuff, iScenList& scList)
{
	uint16 sscnt;
	dbuff.Read(sscnt);
	if (!sscnt) return;
	scList.RemoveAll();
	for (uint16 ssid=0; ssid<sscnt; ++ssid) {
		ScopedPtr<iMapInfo> mapInfo( new iMapInfo() );
		mapInfo->Unserialize(dbuff);
		mapInfo->m_bNewGame = true;
		::Unserialize(dbuff, mapInfo->m_FileName);
		scList.Add(mapInfo.Giveup());
	}
}

/*bool HasMapVersion(iScenList& scList, iStringT name, GAME_LANG lng) {
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

void EnumScenarios(iScenList& scList, uint32 filter)
{
#ifdef MAP_CACHE
	if(iFile::Exists(gMapsPath + _T("maps.cache"))) { // and is valid
		iFilePtr pFile(OpenWin32File(gMapsPath + _T("maps.cache")));
 		check(pFile);
		uint32 fileSize = pFile->GetSize();
		iDynamicBuffer buf(fileSize);
		buf.SetSize( pFile->Read(buf.GetData(),fileSize ) );
		Unserialize(buf, scList);
		return;
	}
#endif


	iStringT strText = gMapsPath + _T("*.phm");
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(strText.CStr(), &wfd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 ) {
				//uint64 size = (wfd.nFileSizeHigh * (1+(uint64)MAXDWORD)) + wfd.nFileSizeLow;
				ScopedPtr<iMapInfo> mapInfo( new iMapInfo() );
				mapInfo->m_bNewGame = true;
				mapInfo->m_FileName = gMapsPath + wfd.cFileName;
#ifdef HMM_COMPOVERSION
				iFilePtr pFile(xxc::OpenXFile(mapInfo->m_FileName, HMM_COMPOCODE));
#else			
				iFilePtr pFile(OpenWin32File(mapInfo->m_FileName));
#endif
				if ( !pFile ) continue;
				check(pFile);
				uint32 fourcc; pFile->Read(&fourcc,sizeof(fourcc));				
				if (fourcc == GMAP_FILE_HDR_KEY && mapInfo->ReadMapInfo(pFile.get(), false)) {											
					if(mapInfo->m_FileName.Find(_T("Tutorial")) != iStringT::NOT_FOUND) {
						if(filter & SCL_TUTORIAL)
							scList.Add(mapInfo.Giveup());					
					} else {
						uint32 hPl = mapInfo->HumanPlayers();
						if((filter & SCL_SINGLE && hPl == 1) ||
							filter & SCL_MULTI && hPl > 1)
							scList.Add(mapInfo.Giveup());					
					}
				}
			}
		} while (FindNextFile(hFind, &wfd));
		FindClose(hFind);
	}
#else
	iStringT strText = gMapsPath + _T("*.phm");
	glob_t g;
	glob(CvtT2A<>(strText.CStr()), GLOB_NOSORT|GLOB_NOESCAPE, NULL, &g);
	for(sint32 x=0; x<g.gl_pathc; x++) {
		ScopedPtr<iMapInfo> mapInfo( new iMapInfo() );
		mapInfo->m_bNewGame = true;
		mapInfo->m_FileName = CvtA2T<>(g.gl_pathv[x]);
		iFilePtr pFile(OpenWin32File(mapInfo->m_FileName));
		//if ( !pFile ) continue;
		check(pFile);
		uint32 fourcc; pFile->Read(&fourcc,sizeof(fourcc));	
		if (fourcc == GMAP_FILE_HDR_KEY && mapInfo->ReadMapInfo(pFile.get(), false)) {											
			if(mapInfo->m_FileName.Find(_T("Tutorial")) != iStringT::NOT_FOUND) {
				if(filter & SCL_TUTORIAL)
					scList.Add(mapInfo.Giveup());					
			} else {
				uint32 hPl = mapInfo->HumanPlayers();
				if((filter & SCL_SINGLE && hPl == 1) ||
				   filter & SCL_MULTI && hPl > 1)
					scList.Add(mapInfo.Giveup());					
			}
		}	
	}
	globfree(&g);
	


#ifdef MAP_CACHE
	// save cache
	iFilePtr pFile(CreateWin32File(gMapsPath + _T("maps.cache")));
	if(pFile.get())
		Serialize(pFile.get(), scList);
#endif 
}

void CleanupScenarioList(iScenList& scList)
{
	for (uint32 xx=0; xx<scList.GetSize(); ++xx) {
		delete scList[xx];
	}
	scList.RemoveAll();

}

/*
 *	Scenario list box
 */
class iScenListBox : public iListBox
{
public:
	iScenListBox(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, const iScenList& scList)
	: iListBox(pViewMgr, pCmdHandler, rect, uid), m_scList(scList)
	{}

	inline uint32 LBItemHeight() const { return 15; }
	inline uint32 LBItemsCount() const { return m_scList.GetSize(); }

private:
	void ComposeLBBackground(const iRect& rect)
	{
		iRect rc = rect;
		gApp.Surface().Darken25Rect(rc);
	}

	void ComposeLBItem(uint32 iIdx, bool bSel, const iRect& irc)
	{
		iTextComposer::FontConfig fc(dlgfc_plain);
		iRect rc=irc;

		ButtonFrame(gApp.Surface(),rc,iButton::Pressed);
		rc.DeflateRect(1);
		if (bSel) {
			gGfxMgr.BlitTile(PDGG_CTILE, gApp.Surface(),rc);
			ButtonFrame(gApp.Surface(),rc,0);
		}

		rc.DeflateRect(1);

		if (!m_scList[iIdx]->Supported()) fc.cmpProps.faceColor = RGB16(192,160,160);

		uint32 btnw = 23;
		
		// Map Size
		gTextComposer.TextOut(fc,gApp.Surface(),rc,gTextMgr[TRID_SHORT_MAPSIZ_SMALL+m_scList[iIdx]->m_Size],iRect(rc.x,rc.y,btnw,rc.h),AlignCenter);
		rc.DeflateRect(btnw,0,0,0);

		// Players count
		gTextComposer.TextOut(fc,gApp.Surface(),rc,iFormat(_T("%d/%d"),m_scList[iIdx]->HumanPlayers(),m_scList[iIdx]->TotalPlayers()),iRect(rc.x,rc.y,btnw,rc.h),AlignCenter);
		rc.DeflateRect(btnw,0,0,0);

		// Map name
		gTextComposer.TextOut(fc,gApp.Surface(),rc,m_scList[iIdx]->MapName(),rc,AlignLeft);
	}

private:
	const iScenList&	m_scList;
};

/*
 *	Scenario List dialog
 */
iScenListDlg::iScenListDlg(iViewMgr* pViewMgr, uint32 filter) 
: iBaseGameDlg(pViewMgr), m_selScen(-1), m_filter(filter), m_scrollMask(0), m_scrollCounter(fix32::zero)
{
}

iScenListDlg::~iScenListDlg()
{
	// store settings ??

	CleanupScenarioList(m_scList);
}

bool iScenListDlg::Process(fix32 t)
{
	m_pLB->Process(t);
		if(m_scrollMask != 0) {
		m_scrollCounter += t;
		if(m_scrollCounter >= fix32(0.1f)) {
			if(m_scrollMask & (0x01)) {
				m_selScen = iCLAMP<sint32>(0, m_scList.GetSize()-1, m_selScen-1);
				m_pLB->SetCurSel(m_selScen, true);
			} else if(m_scrollMask & (0x02)) {
				m_selScen = iCLAMP<sint32>(0, m_scList.GetSize()-1, m_selScen+1);
				m_pLB->SetCurSel(m_selScen, true);
			}
			m_scrollCounter = fix32::zero;
		}
	}
	return false;
}

bool iScenListDlg::KeyDown(sint32 key)
{
	if(key == KEY_UP || key == KEY_DOWN) {
		m_scrollMask |= 0x01 << (key - KEY_UP);
		m_scrollCounter = fix32(1.0f);
	}	
	return false;
}

bool iScenListDlg::KeyUp(sint32 key)
{
	m_scrollMask ^= 0x01 << (key - KEY_UP);
	
	if(key == KEY_ENTER) {
		if (m_selScen != -1 && m_scList[m_selScen]->Supported()) EndDialog(DRC_OK); 
	}
	return false;
}

void iScenListDlg::SortScenarios(SortBy sort_by)
{
	iSortArray<iMapInfo*> sort;
	sint32 idx;
	for (uint32 xx=0; xx<m_scList.GetSize(); ++xx) {
		if (sort_by == Size) idx = m_scList[xx]->m_Size;
		else if (sort_by == RevSize) idx = 010000000 - m_scList[xx]->m_Size;
		else if (sort_by == Players) idx = m_scList[xx]->m_Players.GetSize();
		else if (sort_by == RevPlayers) idx = 010000000 - m_scList[xx]->m_Players.GetSize();
		else if (sort_by == Name) idx = (m_scList[xx]->MapName()[0] << 16) | m_scList[xx]->MapName()[1];			
		else if (sort_by == RevName) idx = 010000000 - ((m_scList[xx]->MapName()[0] << 16) | m_scList[xx]->MapName()[1]);					
		
		if (!m_scList[xx]->Supported()) idx += 0x10000000;
		sort.Insert(m_scList[xx], idx);
	}
	for (uint32 xx=0; xx<m_scList.GetSize(); ++xx) m_scList[xx] = sort.Value(xx);
}

const sint32 yoffs = 18;
void iScenListDlg::OnCreateDlg()
{
	iRect clRect = ClientRect();

	EnumScenarios(m_scList, m_filter);
//#ifndef OS_IPHONE
//	FilterScenarios(m_scList);
//#endif
	SortScenarios(Name);
	
#ifdef OS_IPHONE
	uint32 btnw = 50;
#else
	uint32 btnw = 24;
#endif
	
	// Listbox header
	AddChild(new iDlgIconButton(m_pMgr, this, iRect(clRect.x,clRect.y + yoffs, btnw,DEF_BTN_HEIGHT), PDGG_BTN_MAPSIZE, 501));
	AddChild(new iDlgIconButton(m_pMgr, this, iRect(clRect.x+btnw+1,clRect.y + yoffs, btnw,DEF_BTN_HEIGHT), PDGG_BTN_PLAYERS_COUNT, 502));
	AddChild(new iTextButton(m_pMgr, this, iRect(clRect.x+btnw*2+2,clRect.y + yoffs, clRect.w-16-btnw*2-2,DEF_BTN_HEIGHT), TRID_MAP_NAME, 503));

	// Listbox
	m_pLB = new iScenListBox(m_pMgr, this, iRect(clRect.x,clRect.y + yoffs+DEF_BTN_HEIGHT+1,clRect.w-16,clRect.h - 100), 100, m_scList);
	AddChild(m_pLB);
	// Scroll bar
#ifndef OS_IPHONE
	iPHScrollBar* pScrollBar = new iPHScrollBar(m_pMgr, this, iRect(clRect.x+clRect.w-15, clRect.y + yoffs, 15, 120+DEF_BTN_HEIGHT+1), 300);
	AddChild(pScrollBar);
	m_pLB->SetScrollBar(pScrollBar);
#endif
	// Buttons
	sint32 npos = clRect.x + (clRect.w/2-80);
	AddChild(new iTextButton(m_pMgr,this,iRect(npos,clRect.y2()-DEF_BTN_HEIGHT,50,DEF_BTN_HEIGHT),TRID_OK, DRC_OK,iButton::Visible));
	AddChild(new iTextButton(m_pMgr,this,iRect(npos+55,clRect.y2()-DEF_BTN_HEIGHT,50,DEF_BTN_HEIGHT),TRID_INFO, 301,iButton::Visible));
	AddChild(new iTextButton(m_pMgr,this,iRect(npos+110,clRect.y2()-DEF_BTN_HEIGHT,55,DEF_BTN_HEIGHT),TRID_CANCEL, DRC_CANCEL));
	// Init list
	SortScenarios((SortBy)gSettings.GetEntryValue(CET_NGDSORT));
	if (m_scList.GetSize()) {
		sint32 selScen = iCLAMP<sint32>(0, m_scList.GetSize()-1, gSettings.GetEntryValue(CET_NGDPOS));
		m_pLB->SetCurSel(selScen, true);
	}
}

void iScenListDlg::DoCompose(const iRect& rect) 
{
	gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(),gTextMgr[TRID_SELECT_SCENARIO_DLG_HDR], iRect(rect.x,rect.y,rect.w,24));

	//gApp.Surface().FrameRect(iRect(rect.x-1,rect.y-1+yoffs+DEF_BTN_HEIGHT+1,GetScrRect().w-14,+2),cColor_Black);
}

iSize iScenListDlg::ClientSize() const
{ 
#ifdef OS_IPHONE
	return gApp.Surface().GetSize();
#else
	return iSize(280,150 + DEF_BTN_HEIGHT + DEF_BTN_HEIGHT); 
#endif
}

void iScenListDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param) 
{ 
	uint32 uid = pView->GetUID();
	if (uid == DRC_OK || uid == DRC_CANCEL){
		EndDialog(uid); 
	} else if (uid == 301) {
		iStringT title = m_scList[m_selScen]->MapName();
		//if (!m_scList[m_selScen]->m_Version.Empty()) title.Addf(_T(" v.%s"), m_scList[m_selScen]->m_Version.CStr());
		iStringT desc = m_scList[m_selScen]->MapDescription();
		if (!m_scList[m_selScen]->m_Author.Empty()) desc.Addf(_T("\n\n%s: %s"), gTextMgr[TRID_MAP_AUTHOR], m_scList[m_selScen]->m_Author.CStr());
		iTextDlg tdlg(m_pMgr, title, desc, PID_NEUTRAL, dlgfc_topic, dlgfc_splain);
		tdlg.DoModal();
	} else if (uid == 501) {
		sint32 nval;
		if((SortBy)gSettings.GetEntryValue(CET_NGDSORT) == Size)
			nval = RevSize;
		else
			nval = Size;
		gSettings.SetEntryValue(CET_NGDSORT, nval);
		SortScenarios((SortBy)nval);
		Invalidate();
	} else if (uid == 502) {
		sint32 nval;
		if((SortBy)gSettings.GetEntryValue(CET_NGDSORT) == Players)
			nval = RevPlayers;
		else
			nval = Players;
		gSettings.SetEntryValue(CET_NGDSORT, nval);
		SortScenarios((SortBy)nval);
		Invalidate();
	} else if (uid == 503) {
		sint32 nval;
		if((SortBy)gSettings.GetEntryValue(CET_NGDSORT) == Name)
			nval = RevName;
		else
			nval = Name;
		gSettings.SetEntryValue(CET_NGDSORT, nval);
		SortScenarios((SortBy)nval);
		Invalidate();
	} else if (uid == 100) {
		if (cmd == CCI_LBSELCHANGED) {
			m_selScen = param;
			gSettings.SetEntryValue(CET_NGDPOS, param);
			GetChildById(DRC_OK)->SetEnabled(m_selScen != -1 && m_scList[m_selScen]->Supported() && m_scList[m_selScen]->HumanPlayers() > 0);
			GetChildById(301)->SetEnabled(m_selScen != -1 && m_scList[m_selScen]->HumanPlayers() > 0);
		} else if (cmd == CCI_LBSELDBLCLICK) {
			if (m_selScen != -1 && m_scList[m_selScen]->Supported()) EndDialog(DRC_OK); 
		} 
	}
}

#endif