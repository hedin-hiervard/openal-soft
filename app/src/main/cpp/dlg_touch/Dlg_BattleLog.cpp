#include "stdafx.h"
#include "Dlg_BattleLog.h"

//////////////////////////////////////////////////////////////////////////
class iTextListBox : public iListBox
{
public:
	iTextListBox(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, const iSimpleArray<iStringT>& strList)
	: iListBox(pViewMgr, pCmdHandler, rect, uid), m_strList(strList)
	{}

	inline uint32 LBItemHeight() const { return 15; }
	inline uint32 LBItemsCount() const { return m_strList.GetSize(); }

private:
	void ComposeLBBackground(const iRect& rect)
	{
		iRect rc = rect;
		gApp.Surface().Darken25Rect(rc);
		rc.InflateRect(1);
		gApp.Surface().FrameRect(rc, cColor_Black);
	}

	void ComposeLBItem(uint32 iIdx, bool bSel, const iRect& irc)
	{
		iRect rc=irc;

		if (bSel) {
			gGfxMgr.BlitTile(PDGG(CTILE), gApp.Surface(),rc);
			ButtonFrame(gApp.Surface(),rc,0);
		}

		rc.DeflateRect(2);

		gTextComposer.TextOut(gApp.Surface(), rc, gTextComposer.CropString(m_strList[iIdx], dlgfc_splain, rc.w, true), rc, AlignLeft);
	}

private:
	const iSimpleArray<iStringT>&	m_strList;
};

//////////////////////////////////////////////////////////////////////////
iDlg_BattleLog::iDlg_BattleLog(iViewMgr* pViewMgr, PLAYER_ID pid, const iSimpleArray<iStringT>& log)
: iBaseGameDlg(pViewMgr), m_log(log)
{
}

void iDlg_BattleLog::OnCreateDlg()
{
	iRect clRect = ClientRect();
	AddChild(m_pLogList = new iTextListBox(m_pMgr, this, iRect(clRect.x+5,clRect.y+20,clRect.w-26,135), 101, m_log));
	if(gSettings.GetInterfaceType() == IT_STYLUS) {
		//AddChild(m_pScrollBar = new iPHScrollBar(m_pMgr, this, iRect(clRect.x+clRect.w-20,clRect.y+20,15,135), 102));
		//m_pLogList->SetScrollBar(m_pScrollBar);
	}
	if (m_pLogList->LBItemsCount() > m_pLogList->PageSize()) {
		// scroll list down
		sint32 npos = m_pLogList->LBItemsCount() - m_pLogList->PageSize();
		m_pScrollBar->SetCurPos(npos);
		m_pLogList->SetCurPos(npos);
	}
	//AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-20),clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT),TRID_OK, DRC_OK));
}
#ifdef PC_VERSION
void iDlg_BattleLog::UpdateSize(){

    Center();
    
    iRect clRect = ClientRect();
    m_pLogList->SetRect( iRect(clRect.x+5,clRect.y+20,clRect.w-26,135) );
    
    if (m_pLogList->LBItemsCount() > m_pLogList->PageSize()) {
		// scroll list down
		sint32 npos = m_pLogList->LBItemsCount() - m_pLogList->PageSize();
		m_pScrollBar->SetCurPos(npos);
		m_pLogList->SetCurPos(npos);
	}
}
#endif
void iDlg_BattleLog::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);

	// title
	gTextComposer.TextOut(dlgfc_hdr, gApp.Surface(), rc.point(), gTextMgr[TRID_COMBAT_LOG], iRect(rc.x,rc.y,rc.w,15),AlignCenter);
}

iSize iDlg_BattleLog::ClientSize() const
{
	return iSize(260,165);
}

void iDlg_BattleLog::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if (uid == DRC_OK) {
		EndDialog(pView->GetUID());
	} else if (uid == 101) {
		// Listbox
		if (cmd == CCI_LBSELECTED) {
			if (param != -1) {
				iTextDlg tdlg(m_pMgr, gTextMgr[TRID_COMBAT_LOG], m_log[param], PID_NEUTRAL);
				tdlg.DoModal();
			}
		} 
	} 
}

void iDlg_BattleLog::OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	EndDialog(0);
}

