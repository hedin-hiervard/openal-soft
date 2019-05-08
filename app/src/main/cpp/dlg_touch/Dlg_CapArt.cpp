#include "stdafx.h"
#include "ArtifactList.h"
#include "Dlg_CapArt.h"

#include "lang.h"

const sint32 acW = 32;
const sint32 acH = 32;

iDlg_CapArt::iDlg_CapArt(iViewMgr* pViewMgr, const iHero* pHero, const iArtList& artList)
: iBaseGameDlg(pViewMgr), m_pHero(pHero), m_artList(artList)
{ 
	m_bSingle = artList.GetSize() == 1;
	if (!m_bSingle) m_Title.Setf(gTextMgr[TRID_MSG_CAP_ARTS],m_artList.GetSize());
	else m_Title = gTextMgr[TRID_MSG_CAP_ART];
	
	//Twitts must be in English
	if (!m_bSingle) m_TwitterTitle.Setf(gTextMgr.English(TRID_MSG_CAP_ARTS),m_artList.GetSize());
	else m_TwitterTitle = gTextMgr.English(TRID_MSG_CAP_ART);
}

void iDlg_CapArt::OnCreateDlg()
{
	#ifdef MULTIPLAYER
	if(gMPMgr.IsReplaying()) {
		EndDialog(DRC_NO); // does not fight. if we actually said YES, it would be a separate command to start combat
		return;
	}		
#endif

	iRect clRect = ClientRect();
	iSize sz = gGfxMgr.Dimension(PDGG(TWEET));

	
	if (!m_bSingle) {
        m_pArtCtrl = new iArtListCtrl(
				m_pMgr, this, m_artList, 
				AlignChild(iSize(clRect.w - 10, acH), AlignBottom),
				iIphoneBtn::sm_defSize,
				m_artList.GetSize(),
				iArtBackPackCtrl::Horizontal, 
				101,
				false, iSize(clRect.w - 100, iIphoneBtn::sm_defSize.h));
		AddChild(m_pArtCtrl);
		m_pArtCtrl->SetPos(AlignChild(m_pArtCtrl->GetSize(), AlignBottom) 
#if 0
			- iPoint(0, sz.h)
#endif
			);
	}
#if 0
	AddChild(m_pTweetBtn = new iImgBtn(m_pMgr, this, AlignRect(sz, clRect, AlignBottom) , 100, PDGG(TWEET), PDGG(TWEET_PRESSED)));
#endif
}
#ifdef PC_VERSION
void iDlg_CapArt::UpdateSize(){

    Center();
    
    iRect clRect = ClientRect();
	iSize sz = gGfxMgr.Dimension(PDGG(TWEET));
    
    if (!m_bSingle){
    
        m_pArtCtrl->SetRect( AlignChild(iSize(clRect.w - 10, acH), AlignBottom) );
        m_pArtCtrl->SetPos(AlignChild(m_pArtCtrl->GetSize(), AlignBottom) 
#if 0
			- iPoint(0, sz.h)
#endif
			);
    }
#ifdef OS_MACOS
    m_pTweetBtn->SetRect( AlignRect(sz, clRect, AlignBottom) );
#endif
}
#endif
void iDlg_CapArt::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	rc.y += iBaseGameDlg::GetTopOffset();
	
	rc.y += gTextComposer.TextBoxOut(dlgfc_topic, gApp.Surface(), m_Title, rc);

	if (m_bSingle) {
		
		rc.y += gTextComposer.TextBoxOut(dlgfc_topic, gApp.Surface(), gTextMgr[gGame.ItemMgr().m_artMgr[m_artList[0]].NameKey(m_pHero)], rc);
		rc.y += gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), gTextMgr[gGame.ItemMgr().m_artMgr[m_artList[0]].DescKey(m_pHero)], rc);
		iRect arc = AlignRect(iSize(32, 32), rc, AlignTop);
		BlitIcon(gApp.Surface(), gGame.ItemMgr().m_artMgr[m_artList[0]].Sprite(), arc);

	}
}

iSize iDlg_CapArt::ClientSize() const
{
	sint32 w = (m_bSingle)?iBaseGameDlg::GetDefaultWidth():iCLAMP<sint32>(iBaseGameDlg::GetDefaultWidth(), gApp.Surface().GetWidth(), m_artList.GetSize()*acW+20);
	sint32 h = iGlowGameDlg::GetTopOffset();


	// title
	h += gTextComposer.GetTextBoxSize(m_Title, w, dlgfc_topic).h;

	if (m_bSingle) {
		// Artifact icon
		h += 32;
		// Artifact name
		h += gTextComposer.GetTextBoxSize(gTextMgr[gGame.ItemMgr().m_artMgr[m_artList[0]].NameKey(m_pHero)], w, dlgfc_topic).h;
		// Artifact description
		h += gTextComposer.GetTextBoxSize(gTextMgr[gGame.ItemMgr().m_artMgr[m_artList[0]].DescKey(m_pHero)], w, dlgfc_plain).h;
	} else {
		// Artifact list
		h += 60;
	}
#if 0
	iSize sz = gGfxMgr.Dimension(PDGG(TWEET));
	h += sz.h;
#endif
	return iSize(w,h);
}

void iDlg_CapArt::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if(uid == 100) {
		return;
	} 
	
	EndDialog(pView->GetUID());
}

void iDlg_CapArt::OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{	
	EndDialog(DRC_OK);
}





