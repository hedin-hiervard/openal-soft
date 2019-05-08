#ifdef MULTIPLAYER

#include "stdafx.h"
#include "Dlg_ScenProps.h"
#include "MPGameSetupView.h"

iMPGameSetupView::ParticipantItem::ParticipantItem(iViewMgr* pViewMgr, uint32 uid, const iRect &rect, PLAYER_ID pid)
: iView(pViewMgr, rect, GENERIC_VIEWPORT, uid, Visible | Enabled), m_pid(pid)
{	
	AddChild(m_pPlayerBtn = new iPlayerBtn(pViewMgr, this, iRect(0, 0, 34, 34), pid, PT_HUMAN, 100, Visible|Enabled));
	//AddChild(m_pNationBtn = new iNationBtn(pViewMgr, iRect(50, 0, 34, 22), pid, CTLT_CITADEL, 101, false));
	//m_user.m_id = cInvalidUser;
}

void iMPGameSetupView::ParticipantItem::UpdateData(const iMPManager::GameUserData& data)
{
	m_user = data;
//	m_pNationBtn->SetNation(data.m_castle);
}

void iMPGameSetupView::ParticipantItem::OnCompose() 
{
	iPoint offset((gApp.Surface().GetWidth() - 320) / 2, (gApp.Surface().GetHeight() - 240) / 2);
	iPoint pt = GetScrPos() + iPoint(40, 10);
	if(m_user.m_nickname != ("")) {
		iStringT tmp = iStringT(CvtA2T<>(m_user.m_nickname.CStr()));
		gTextComposer.TextOut(gApp.Surface(), pt, tmp);
	}
}

void iMPGameSetupView::ParticipantItem::iCMDH_ControlCommand(iView *pVIew, CTRL_CMD_ID cmd, sint32 param) 
{
}

iMPGameSetupView::iMPGameSetupView(): iChildGameView(false, iChildGameView::CV_MENU) 
{
	iPoint offset((gApp.Surface().GetWidth() - 320) / 2, (gApp.Surface().GetHeight() - 240) / 2);
	iRect rc = iRect(offset + iPoint(55, 80), iSize(105, 30));
	
	
	if(gMPMgr.GetHostType() == iMPManager::HT_HOST) {
		const iMapInfo& scProps = gMPMgr.MapInfo();
		for(uint32 x=0; x<scProps.TotalPlayers(); x++) {
			rc.x = offset.x + 55 + (x%2 * 110);
			rc.y = offset.y + (x / 2) * 32 + 80;
			AddChild(new ParticipantItem(&gApp.ViewMgr(), 110+scProps.m_Players[x].m_Id, rc, scProps.m_Players[x].m_Id));			
		}
	}
	
	if(gMPMgr.GetHostType() == iMPManager::HT_HOST)
		m_state = SETUP_HOST;
	else
		m_state = SETUP_CLIENT;

	AddChild(m_pStartBtn = new iTextButton(&gApp.ViewMgr(), this, iRect(offset + iPoint(5, 210), iSize(150, DEF_BTN_HEIGHT)), m_state == SETUP_CLIENT ? TRID_READY : TRID_LAUNCH, 101, m_state == SETUP_CLIENT ? (Enabled): (Visible)));	
	AddChild(new iTextButton(&gApp.ViewMgr(), this, iRect(offset + iPoint(165, 210), iSize(150, DEF_BTN_HEIGHT)), TRID_CANCEL, 103, Visible | Enabled));	
}

iMPGameSetupView::~iMPGameSetupView() {}

void iMPGameSetupView::UpdatePlayers()
{
	const iMPManager::GameUserList& list = gMPMgr.GetGameUserList();	
	iMPManager::GameUserData data;
	data.m_castle = CTLT_CITADEL;
	data.m_color = PID_RED;
	data.m_nickname = ("");
	data.m_id = 0xFFFFFF;
	iPoint offset((gApp.Surface().GetWidth() - 320) / 2, (gApp.Surface().GetHeight() - 240) / 2);
	for(uint32 xx=0; xx<PID_COUNT;xx++)
	{
		ParticipantItem *it = dynamic_cast<ParticipantItem*>(GetChildById(110 + xx));
		if(it)
			it->UpdateData(data);
	}

	uint32 count = 0;
	for(uint32 xx=0; xx<PID_COUNT; xx++)
		if(gMPMgr.PlayersPresent()[xx]) {
			ParticipantItem *it = dynamic_cast<ParticipantItem*>(GetChildById(110 + xx));
			if(!it) {
				iRect rc(offset + iPoint(55 + (count%2 * 110), 80 + (count / 2) * 32), iSize(105, 30));
				AddChild(it = new ParticipantItem(&gApp.ViewMgr(), 110+xx, rc, (PLAYER_ID)xx));			
				count++;
			}
		}

	for(uint32 x=0; x<list.GetSize(); x++)
	{
		ParticipantItem *it = dynamic_cast<ParticipantItem*>(GetChildById(110 + list[x].m_color));
		if(it) {
			it->UpdateData(list[x]);			
		}		
	}
	
	Invalidate();
	SetActive(true);
}

void iMPGameSetupView::OnCompose()
{
	gApp.Surface().FillRect(iRect(iPoint(), gApp.Surface().GetSize()), RGB16(0,0,0));
	iPoint offset((gApp.Surface().GetWidth() - 320) / 2, (gApp.Surface().GetHeight() - 240) / 2);
	gGfxMgr.Blit(PDGG(MENU_BACK), gApp.Surface(), offset);	
}

void iMPGameSetupView::iCMDH_ControlCommand(iView *pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if(uid == 101) {
		switch(m_state) {
		case SETUP_CLIENT:
			m_state = READY_CLIENT;
			m_pStartBtn->SetEnabled(false);
			gMPMgr.MarkReady();
			break;
		case SETUP_HOST:
			//if(gMPMgr.GetGameUserList().GetSize() > 1)
				gMPMgr.LaunchGame();
			/*else {
				iTextDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_UNABLE_TO_LAUNCH_FEW_PLAYERS], _T(""),
					PID_NEUTRAL);				
				dlg.DoModal();			
			}*/
			break;
		}
	} else if(uid == 102) {

	} else if(uid == 103) {
		gMPMgr.LeaveGame();
	}
}

void iMPGameSetupView::SetActive(bool bAct)
{
	m_pStartBtn->SetEnabled(bAct);	
	for(uint32 xx=0; xx<PID_COUNT; xx++)
	{
		ParticipantItem *it = dynamic_cast<ParticipantItem*>(GetChildById(110 + xx));
		if(it) {
			it->SetEnabled(bAct);
		}
	}
}

#endif //MULTIPLAYER