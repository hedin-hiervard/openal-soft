//
// Dlg_Save.cpp
//

#include "stdafx.h"
#include "Dlg_Save.h"
#include "helpers.h"
#include "Ctl_Save.h"

#include "FileAccessor/FileAccessor.h"

//////////////////////////////////////////////////////////////////////////
iSaveDlg::iSaveDlg(iViewMgr* pViewMgr, bool bSave)
: iBaseGameDlg(pViewMgr), m_bSave(bSave)
{}

iSaveDlg::~iSaveDlg()
{

}

void iSaveDlg::OnCreateDlg()
{
    iRect clRect = ClientRect();
    sint32 yp = clRect.y;

	clRect.h = 341;
	clRect.w = 510;

    //clRect.DeflateSize(50);
    AddChild(m_ctrl = new iSaveGameView(m_pMgr, this, clRect, 109, m_bSave) );
}


iSize iSaveDlg::ClientSize()  const
{
    return gApp.Surface().GetSize();
}

void iSaveDlg::DoCompose(const iRect& rect)
{
    iRect rc(rect);
}

void iSaveDlg::GetSaveFileName(fileaccessor::RelativeFilePath& path, uint32 slot)
{
    iSaveGameView::GetSaveFileName(path, slot);
}

bool iSaveDlg::SelFile(fileaccessor::RelativeFilePath& path) const
{
    return m_ctrl->SelFile(path);
}

const iMapInfo& iSaveDlg::SelScenario() const
{
    return m_ctrl->SelScenario();
}

void iSaveDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
    uint32 uid = pView->GetUID();
    if (uid == 109)
    {
        // End dialog
        EndDialog(param);
    }
}


#if 0
/*
 *
 */
void EnumSaveGames(iSaveSlots& saveSlots)
{
	for (uint32 xx=0; xx<SAVE_GAME_SLOTS; ++xx) {
		iStringT fname = gSavePath + iFormat(_T("save%02d.phsd"),xx+1);
#ifdef HMM_COMPOVERSION
		iFilePtr pFile(xxc::OpenXFile(fname.CStr(),HMM_COMPOCODE));
#else
		iFilePtr pFile(OpenWin32File(fname.CStr()));
#endif
		if (pFile) {
			iMapInfo* pMapInfo = new iMapInfo;
			pMapInfo->m_bNewGame = false;
			pMapInfo->m_FileName = fname;
			uint32 fourcc; pFile->Read(&fourcc,sizeof(fourcc));
			if (fourcc == GMAP_FILE_HDR_KEY && pMapInfo->ReadMapInfo(pFile.get())) {
				saveSlots.Add(pMapInfo);
				pFile.reset();
				continue;
			}
			delete pMapInfo;
		}
		saveSlots.Add(NULL);
	}
}

/*
 *	Scenario list box
 */
class iSaveListBox : public iListBox
{
public:
	iSaveListBox(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, const iSaveSlots& saveSlots)
	: iListBox(pViewMgr, pCmdHandler, rect, uid), m_saveSlots(saveSlots)
	{}

	inline uint32 LBItemHeight() const { return 28; }
	inline uint32 LBItemsCount() const { return m_saveSlots.GetSize(); }

private:
	void ComposeLBBackground(const iRect& rect)
	{
		iRect rc = rect;
		gApp.Surface().Darken25Rect(rc);
	}

	void ComposeLBItem(uint32 iIdx, bool bSel, const iRect& irc)
	{
		iRect rc=irc;

		ButtonFrame(gApp.Surface(),rc,iButton::Pressed);
		rc.DeflateRect(1);
		if (bSel) {
			gGfxMgr.BlitTile(PDGG(CTILE), gApp.Surface(),rc);
			ButtonFrame(gApp.Surface(),rc,0);
		}

		rc = irc;
		if (m_saveSlots[iIdx]) {
			// Map Size
			gTextComposer.TextOut(dlgfc_splain ,gApp.Surface(),rc,gTextMgr[TRID_SHORT_MAPSIZ_SMALL+m_saveSlots[iIdx]->m_Size],iRect(rc.x,rc.y,20,rc.h),AlignCenter);
			rc.DeflateRect(20,0,0,0);

			// Players count
			gTextComposer.TextOut(dlgfc_splain,gApp.Surface(),rc,iFormat(_T("%d/%d"),m_saveSlots[iIdx]->HumanPlayers(),m_saveSlots[iIdx]->TotalPlayers()),iRect(rc.x+rc.w-25,rc.y,25,rc.h),AlignCenter);
			rc.DeflateRect(0,0,25,0);

			// Map name
			gTextComposer.TextOut(dlgfc_stopic,gApp.Surface(),rc,m_saveSlots[iIdx]->MapName(),rc,AlignLeft);
		} else {
			gTextComposer.TextOut(dlgfc_splain,gApp.Surface(),rc,gTextMgr[TRID_EMPTY],rc,AlignCenter);
		}
	}

private:
	const iSaveSlots&	m_saveSlots;
};

/*
 *	Save dialog
 */
iSaveDlg::iSaveDlg(iViewMgr* pViewMgr, bool bSave)
: iBaseGameDlg(pViewMgr), m_bSave(bSave), m_selSlot(-1)
{
}

iSaveDlg::~iSaveDlg()
{
	for (uint32 xx=0; xx<m_saveSlots.GetSize(); ++xx) {
		if (m_saveSlots[xx]) delete m_saveSlots[xx];
	}
	m_saveSlots.RemoveAll();
}

void iSaveDlg::GetSaveFileName(iStringT& fname, uint32 slot)
{
	fname = gSavePath + iFormat(_T("save%02d.phsd"),slot+1);
}

void iSaveDlg::SelFile(iStringT& fname) const
{
	check(m_selSlot >= 0  && m_selSlot < SAVE_GAME_SLOTS);
	GetSaveFileName(fname, m_selSlot);
}

const iMapInfo& iSaveDlg::SelScenario() const
{
	check(m_selSlot >= 0  && m_selSlot < SAVE_GAME_SLOTS && m_saveSlots[m_selSlot]);
	return *m_saveSlots[m_selSlot];
}

void iSaveDlg::OnCreateDlg()
{
	iRect clRect = ClientRect();

	EnumSaveGames(m_saveSlots);
	AddChild(new iSaveListBox(m_pMgr, this, iRect(clRect.x,clRect.y+22,clRect.w-150,clRect.h - 57), 100, m_saveSlots));

	sint32 npos = clRect.x + (clRect.w/2-100);
	AddChild(new iTextButton(m_pMgr,this,iRect(npos,clRect.y2()-DEF_BTN_HEIGHT + 3,100,DEF_BTN_HEIGHT),(m_bSave)?TRID_SAVE:TRID_LOAD, DRC_OK,iButton::Visible));
	AddChild(new iTextButton(m_pMgr,this,iRect(npos+110,clRect.y2()-DEF_BTN_HEIGHT + 3,100,DEF_BTN_HEIGHT),TRID_CANCEL, DRC_CANCEL));
}

void iSaveDlg::DoCompose(const iRect& rect)
{
	gTextComposer.TextOut(dlgfc_hdr, gApp.Surface(), rect, gTextMgr[(m_bSave)?TRID_SAVE_DLG_HDR:TRID_LOAD_DLG_HDR], rect, AlignTop);
	//gApp.Surface().FrameRect(iRect(rect.x-1,rect.y+22-1,252,176),cColor_Black);
	iRect orc(rect.x + rect.w - 150, rect.y+21, 151, 176);


	// Compose selected map information
	orc.DeflateRect(1);
	gApp.Surface().Darken25Rect(orc);
	if (m_selSlot != -1 && m_saveSlots[m_selSlot] != NULL) {
		orc.y += 10;

		iTextComposer::FontSize fs = iTextComposer::FS_LARGE;
		iTextComposer::FontConfig fc_hdr(fs, iDibFont::ComposeProps(RGB16(255,255,128)));
		iTextComposer::FontConfig fc_text(fs, iDibFont::ComposeProps(RGB16(192,192,192)));

		gTextComposer.TextOut(fc_hdr, gApp.Surface(), orc, gTextMgr[TRID_DIFFICULTY_LEVEL], orc, AlignTop); orc.y += 15;
		gTextComposer.TextOut(fc_text, gApp.Surface(), orc, gTextMgr[TRID_DIFF_EASY+m_saveSlots[m_selSlot]->m_Difficulty], orc, AlignTop); orc.y += 19;

		gTextComposer.TextOut(fc_hdr, gApp.Surface(), orc, gTextMgr[TRID_GAME_DATE], orc, AlignTop); orc.y += 15;
		sint32 days = m_saveSlots[m_selSlot]->m_curDay - 1;
		gTextComposer.TextOut(fc_text, gApp.Surface(), orc, iFormat(_T("%s:%d / %s:%d / %s:%d"),gTextMgr[TRID_MONTH_S], days/28+1,gTextMgr[TRID_WEEK_S], (days%28)/7+1,gTextMgr[TRID_DAY_S],days%7+1), orc, AlignTop); orc.y += 19;

		gTextComposer.TextOut(fc_hdr, gApp.Surface(), orc, gTextMgr[TRID_SAVED_AT], orc, AlignTop); orc.y += 15;
		gTextComposer.TextOut(fc_text, gApp.Surface(), orc, FormatDate(m_saveSlots[m_selSlot]->m_saveTime, true), orc, AlignTop); orc.y += 19;
	} else {
		gTextComposer.TextOut(dlgfc_plain, gApp.Surface(), rect, _T("-"), orc, AlignCenter);
	}
}

iSize iSaveDlg::ClientSize()  const
{
	return gApp.Surface().GetSize();
}

void iSaveDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if (uid == DRC_OK){
		if (CanSelect() && Confirmed()) {
			EndDialog(DRC_OK);
		}
	} else if (uid == DRC_CANCEL) {
		EndDialog(DRC_CANCEL);
	} else if (uid == 100) {
		if (cmd == CCI_LBSELCHANGED) {
			m_selSlot = param;
			GetChildById(DRC_OK)->SetEnabled(CanSelect());
		} else if (cmd == CCI_LBSELECTED) {
			if (CanSelect() && Confirmed()) {
				EndDialog(DRC_OK);
			}
		}
	}
}

bool iSaveDlg::Confirmed() const
{
	check(m_selSlot != -1);
	if (m_bSave && m_saveSlots[m_selSlot] != NULL) {
		iQuestDlg qdlg(m_pMgr, _T(""), gTextMgr[TRID_CONFIRM_OVERWRITE], PID_NEUTRAL);
		if (qdlg.DoModal() == DRC_NO) return false;
	}
	return true;
}

#endif
