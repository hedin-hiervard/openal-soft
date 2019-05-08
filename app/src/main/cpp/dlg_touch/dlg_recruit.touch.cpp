#include "stdafx.h"
#include "CastleView.h"
#include "Dlg_Recruit.touch.h"
#include "Dlg_CreatInfo.h"

iDlg_Recruit::iDlg_Recruit(iViewMgr* pViewMgr, iCreatGroup& creats, const iArmy& army, PLAYER_ID owner, iHero *pHero):
iBaseGameDlg(pViewMgr), m_pid(owner)
, m_creats(creats), m_army(army), m_avail(0), m_recr(0), m_max(0), m_pHero(pHero)
{ }

void iDlg_Recruit::OnCreateDlg()
{
#ifdef MULTIPLAYER
	if(gMPMgr.IsReplaying()) {
		EndDialog(DRC_OK);
		return;
	}		
#endif

	m_avail = m_creats.Count();
	check(m_pid != PID_NEUTRAL);
	
	iPlayer* pOwner = gGame.Map().FindPlayer(m_pid);
	check(pOwner);
	m_crCost = CREAT_DESC[m_creats.Type()].cost;
	m_max = iMIN<sint32>(m_avail, pOwner->Minerals().Has(m_crCost));

	/*
	iMineralSet ms = m_crCost;
	while (m_max < m_avail && pOwner->Minerals().Has(ms)) {
		ms += m_crCost;
		++m_max;
	}*/	

	sint32 h = iBaseGameDlg::GetTopOffset();

	
	AddChild(m_pDesc = new iCreatDescView(m_pMgr, this, iRect(), 105, CDV_COST | CDV_TITLE | CDV_GLOW 
#ifdef PC_VERSION
                                          | CDV_LINEAR_PERKS
#endif
                                          , NULL));
	m_pDesc->SetRect(iRect(iPoint(0, h), m_pDesc->GetSize(m_Rect.w, m_creats.Type(), 0, CDV_COST | CDV_TITLE | CDV_GLOW 
#ifdef PC_VERSION
                                                          | CDV_LINEAR_PERKS
#endif
                                                          , NULL)));
	m_pDesc->SetType(m_creats.Type());
	m_pDesc->SetAmount(0);
	m_pDesc->SetMinerals(&m_pHero->Owner()->Minerals());

	iRect rct = AlignChild(iAmountChooserGlow::sm_defSize, AlignBottom);
    
	AddChild(m_pAmount = new iAmountChooserGlow(m_pMgr, this, rct, 101
//#ifndef PC_VERSION
                                                , true
//#endif
                                                ));
	m_pAmount->UpdateSize();
	m_pAmount->SetPos(iPoint(m_pAmount->GetPos().x, m_pDesc->GetRect().y2()));
	m_pAmount->SetMax(m_max);

//	h += m_pDesc->GetSize(m_Rect.w, m_creats.Type(), 0, CDV_COST | CDV_TITLE | CDV_GLOW, NULL).h;

	sint32 bw = iIphoneBtn::sm_defSizeDeluxe.w;
#ifdef PC_VERSION
	AddChild(new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom) + iPoint(bw, 0), DRC_OK, PDGG(BTN_BUY_DELUXE), -1));
    AddChild(new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom) - iPoint(bw, 0), DRC_CANCEL, PDGG(BTN_CANCEL_DELUXE), -1));
#else
    AddChild(new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom) + iPoint(3 * bw / 2, rct.h-iIphoneBtn::sm_defSize.h/2 - 30), DRC_OK, PDGG(BTN_BUY_DELUXE), PDGG(BTN_DELUXE_PRESSED)));
    AddChild(new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom) + iPoint(0, rct.h-iIphoneBtn::sm_defSize.h/2 - 30), 200, PDGG(BTN_INFO_DELUXE), -1));
	AddChild(new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom) + iPoint(-3 * bw / 2, rct.h-iIphoneBtn::sm_defSize.h/2 - 30), DRC_CANCEL, PDGG(BTN_CANCEL_DELUXE), PDGG(BTN_DELUXE_PRESSED)));
#endif
	GetChildById(DRC_OK)->SetVisible(m_recr>0);
}

void iDlg_Recruit::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
    
    if(gUseIpadUI)
    {
        iMineralSet ms = m_pHero->Owner()->Minerals();
        iRect trc = AlignRect(GetMineralSetComposeSize(ms, false, true, true), rc, AlignBottom) - iPoint(0, 70);
        ComposeMineralSet(ms, ms, trc, false, false, false, true, true);
    }
    
//	gApp.Surface().FrameRect(rc, cColor_Red);
}

iSize iDlg_Recruit::ClientSize() const
{
	sint32 w = iBaseGameDlg::GetDefaultWidth();
	sint32 h = iBaseGameDlg::GetTopOffset();

	// creature description
	h += iCreatDescView::GetSize(w, m_creats.Type(), m_max, CDV_TITLE | CDV_COST  
#ifdef PC_VERSION
                                 | CDV_LINEAR_PERKS
#endif
                                 , NULL).h;
#ifndef PC_VERSION
	h += iBaseGameDlg::GetTopOffset();
#endif
	// slider
	h += iAmountChooserGlow::sm_defSize.h;
#ifndef PC_VERSION
	h += iBaseGameDlg::GetTopOffset() * 3;
#endif
	// buttons
    if(!gUseIpadUI)
        h += DEF_BTN_HEIGHT;
    else
        h += 2*DEF_BTN_HEIGHT;
    h += 30; // resources
    
	return iSize(w, h);
}
void iDlg_Recruit::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	if (!IsValidDialog()) return;
	uint32 uid = pView->GetUID();
	if (uid == 101) {
		m_recr = m_pAmount->CurPos();
		m_pDesc->SetAmount(m_recr);
		GetChildById(DRC_OK)->SetVisible(m_recr>0);
	} else {
		if (uid == 100) {
			m_pAmount->SetCurPos(m_max);
			m_recr = m_max;
			GetChildById(DRC_OK)->SetVisible(m_recr>0);
		} else if (uid == 201) {
			iCreatGroup group(m_creats.Type(),m_avail);
			iCreatBonusList cbl;
			if(m_pHero)
				m_pHero->Artifacts().HasCreatBonus(m_creats.Type(), cbl);
			// fake army owner
			iArmyOwner fake;
			fake.ResetArmy();
			fake.AddGroupToArmy(m_creats.Type(), 1);
			iDlg_CreatInfo cidlg(m_pMgr, m_pid, &fake, 0, iFurtSkills(), false, 0, cbl);
			cidlg.DoModal();
		} else if (uid == DRC_OK) {
			// Check space in target army
			if (m_army.CanAddGroup(m_creats.Type())) {
				m_pHero->AddGroupToArmy(m_creats.Type(), m_recr);
				m_creats.Count() -= m_recr;
				gGame.Map().FindPlayer(m_pid)->AddMineralSet(-m_crCost * m_recr);
				EndDialog(DRC_OK);
			} else {
				iTextDlg tdlg(m_pMgr, _T(""), gTextMgr[TRID_MSG_NO_ROOM], m_pid);
				tdlg.DoModal();
			}
		} else if (uid == DRC_CANCEL) {
			EndDialog(DRC_CANCEL);
		} else if(uid == 200) {
            iArmyOwner fake;
            CREATURE_TYPE ct = m_creats.Type();
            if(ct == CREAT_UNKNOWN) return;
            fake.AddGroupToArmy(ct, 1);		
            iDlg_CreatInfo dlg(&gApp.ViewMgr(), PID_NEUTRAL, &fake, 0, iFurtSkills(), false, 0, iCreatBonusList());
            dlg.DoModal();
        }
	}
}

void iDlg_Recruit::OnMouseWheelDown(sint16 line, MouseId mID){

    sint32 pos = m_pAmount->CurPos();
    pos += iSIGN(line);
    
    m_pAmount->SetCurPos(pos);
    
    m_recr = m_pAmount->CurPos();
    m_pDesc->SetAmount(m_recr);
    
    GetChildById(DRC_OK)->SetVisible(m_recr>0);
    
//    Update();
    Invalidate();
}
