#include "stdafx.h"
#include "Dlg_TownList.h"

//////////////////////////////////////////////////////////////////////////
class iTownListBox : public iListBoxEx
{
public:
	enum State {
		Normal,
		Occupied,
	};

	iTownListBox(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, iPlayer* pOwner)
	: iListBoxEx(pViewMgr, pCmdHandler, rect, gApp.Surface(), uid), m_pOwner(pOwner)
	{}

	State GetItemState(sint32 idx) {
		check(idx >=0 && idx < (sint32)m_pOwner->CastleCount());
		iCastle* pCastle = *(m_pOwner->CastleFirst()+idx);
		return (pCastle->Visitor())?Occupied:Normal;
	}

	inline uint32 LBItemHeight() const { 
		return gTextComposer.GetFontHeight(dlgfc_topic.fontSize) * 2;
	}
	inline uint32 LBItemsCount() const { 
    
        uint32 count = 0;
        
        iPlayer::iCtLIt begin = m_pOwner->CastleFirst(), end = m_pOwner->CastleEnd();
        
        for( ;begin!=end; begin++ ){
        
            if( !(*begin)->Visitor() )
                count++;
        }
        
        return count;//m_pOwner->CastleCount(); 
    }

private:
	void ComposeLBBackground(const iRect& rect)
	{
		iRect rc = rect;
		gApp.Surface().Darken25Rect(rc);
		rc.InflateRect(1);
		gApp.Surface().FrameRect(rc, cColor_Black);
	}

	void ExComposeLBItem(iDib &dib, sint32 iIdx, bool bSel, const iRect& irc)    	
	{
        State state = GetItemState(iIdx);
        
        if( state == Occupied )
            return;
        
		iTextComposer::FontConfig fc(dlgfc_plain);
		iRect rc=irc;

		if (bSel) {
			dib.FillRect(rc, cColor_Gray128);
		}

		rc.DeflateRect(2);
		iCastle* pCastle = *(m_pOwner->CastleFirst()+iIdx);

		// Glyph
//		gApp.Surface().FillRect(iRect(rc.x,rc.y,34,22),cColor_Black);
		SpriteId sid = PDGG(BTN_CASTLES) + pCastle->Proto()->Type();
		gGfxMgr.Blit(sid, dib, iPoint(rc.x+1,rc.y + 
												10
												));

		// Name
		rc.DeflateRect(36,0,0,0);
		rc.h = gTextComposer.GetFontHeight(dlgfc_topic.fontSize);
		gTextComposer.TextOut(dlgfc_topic,dib,rc,pCastle->Name(),rc,AlignTop);
//		State state = GetItemState(iIdx);

		rc.y+=gTextComposer.GetFontHeight(dlgfc_topic.fontSize);;
		if (state == Occupied) {			
			gTextComposer.TextOut(listfc_text,dib,rc,pCastle->Visitor()->Name(),rc,AlignTop);
		} else if (state == Normal) {			
			gTextComposer.TextOut(listfc_text, dib,rc,gTextMgr[TRID_UNOCCUPIED],rc,AlignTop);
		} else {
			check(0);
		}
	}

private:
	iPlayer*	m_pOwner;
};

//////////////////////////////////////////////////////////////////////////
iDlg_TownList::iDlg_TownList(iViewMgr* pViewMgr, iPlayer* pOwner, const iStringT& title, TextResId okBtnText, bool bCanCancel)
: iBaseGameDlg(pViewMgr), m_pOwner(pOwner), m_title(title), m_okBtnText(okBtnText), m_pDestination(NULL), m_bCanCancel(bCanCancel)
{}

bool iDlg_TownList::Process(fix32 t)
{ 

	m_pTownList->Process(t); 
	return true;
}


void iDlg_TownList::OnCreateDlg()
{
	iRect clRect = ClientRect();

	AddChild(m_pTownList = new iTownListBox(m_pMgr, this, iRect(clRect.x+5, clRect.y+gTextComposer.GetFontHeight(dlgfc_topic.fontSize)+5, clRect.w-5-1, (gApp.Surface().GetHeight() / 2 -50 )), 101, m_pOwner));
	sint32 npos = clRect.x + (clRect.w/2-65);
    sint32 bw = iIphoneBtn::sm_defSize.w;

//	AddChild(m_pbtnOk =  new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) + iPoint(bw, 0),m_okBtnText, DRC_OK, Visible));
	if(m_bCanCancel)
        AddChild(m_pBtnCancel = new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom), DRC_CANCEL, PDGG(BTN_CANCEL_DELUXE), -1));
}
#ifdef PC_VERSION
void iDlg_TownList::UpdateSize(){

    Center();
    
    iRect clRect = ClientRect();
    
    m_pTownList->SetRect( iRect(clRect.x+5, clRect.y+gTextComposer.GetFontHeight(dlgfc_topic.fontSize)+5, clRect.w-5-1, (gApp.Surface().GetHeight() / 2 -50 )) );
    
    sint32 bw = iMAX<>(gTextComposer.GetTextSize(gTextMgr[TRID_OK], btnfc_normal).w, gTextComposer.GetTextSize(gTextMgr[TRID_CANCEL], btnfc_normal).w) + 30;
    
    if(m_bCanCancel)
        m_pBtnCancel->SetRect( AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) );
}
#endif
void iDlg_TownList::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	// title
	gTextComposer.TextOut(dlgfc_hdr, gApp.Surface(),rc.point(), m_title, iRect(rc.x,rc.y,rc.w,15),AlignTop);
	rc.y+=15;
}

iSize iDlg_TownList::ClientSize() const
{
	sint32 w = iBaseGameDlg::GetDefaultWidth();
	sint32 h = iBaseGameDlg::GetTopOffset();

	// title
	h += gTextComposer.GetFontHeight(dlgfc_topic.fontSize);
	// town list
	h += 5+(gApp.Surface().GetHeight() / 2 -50 )+5+gTextComposer.GetFontHeight(dlgfc_topic.fontSize)+5;
	// buttons
	h += DEF_BTN_HEIGHT;

	return iSize(w,h);
}

void iDlg_TownList::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if (uid == DRC_CANCEL) {
		EndDialog(uid);
	} else if (uid == DRC_OK) {
		sint32 sel = m_pTownList->SelItem();
		check(m_pTownList->GetItemState(sel) == iTownListBox::Normal);
		m_pDestination = *(m_pOwner->CastleFirst() + sel);
		EndDialog(uid);
	} else if (uid == 101) {
		// Listbox
		if (cmd == CCI_LBSELCHANGED) {
			//m_pbtnOk->SetEnabled(param != -1 && m_pTownList->GetItemState(param) == iTownListBox::Normal);
		} else if (cmd == CCI_LBSELECTED && param != -1) {
			check(param != -1 && param < (sint32)m_pOwner->CastleCount());
			if (m_pTownList->GetItemState(param) == iTownListBox::Normal) {
				m_pDestination = *(m_pOwner->CastleFirst() + param);
				EndDialog(DRC_OK);
			}
		}
	}
}


