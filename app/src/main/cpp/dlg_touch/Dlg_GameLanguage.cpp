#include "stdafx.h"
#include "lang.h"
#include "OverlandView.h"

#include "Dlg_GameLanguage.h"


sint16 lang_icons[GLNG_COUNT] = {
	PDGG_SINT16(FLAG_US),
	PDGG_SINT16(FLAG_RU),
	PDGG_SINT16(FLAG_PL),
	PDGG_SINT16(FLAG_SK),
	PDGG_SINT16(FLAG_GE),
	PDGG_SINT16(FLAG_CZ),
	-1,
	PDGG_SINT16(FLAG_FR),
	-1
};

class iLanguageSelectView: public iView, public IViewCmdHandler {
public:
	iLanguageSelectView(iViewMgr* pViewMgr, IViewCmdHandler* pHandler, const iRect& rc, uint32 uid): iView(pViewMgr, rc, GENERIC_VIEWPORT, uid, Visible|Enabled),
		m_pHandler(pHandler)
	{		
		uint32 hrln = ((uint32)(rc.w / iIphoneBtn::sm_defSize.w) - 1);
		uint32 cnt = 0;
		for(uint32 xx=0; xx<GLNG_COUNT; xx++) 
			if(lang_icons[xx] != -1)
				cnt++;

		uint32 xoff = rc.w / hrln;
		uint32 yoff = rc.h / (ceil(((double)cnt / hrln)));
		iRect trc = iRect(iPoint((xoff  - iIphoneBtn::sm_defSize.w) / 2, (yoff - iIphoneBtn::sm_defSize.h)/ 2), iIphoneBtn::sm_defSize);

		for(uint32 xx=0; xx<GLNG_COUNT; xx++) 
			if(lang_icons[xx] != -1)
			{
				AddChild(new iIphoneBtn(m_pMgr, this, trc, 100 + xx, Visible | Enabled, lang_icons[xx]));
				trc.x += xoff;
				if(trc.x2() >= rc.w) {
					trc.x = (xoff  - iIphoneBtn::sm_defSize.w) / 2;
					trc.y += yoff;
				}
			}		
	}

	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param) {
		m_pHandler->iCMDH_ControlCommand(this, cmd, pView->GetUID() - 100);
	}
	
	void UpdateSize(){
	
		
	}
private:
	IViewCmdHandler *m_pHandler;

};

iDlg_GameLanguage::iDlg_GameLanguage(iViewMgr* pViewMgr)
: iDialog(pViewMgr)
{}

void iDlg_GameLanguage::OnCreateDlg()
{
	iRect clRect = GetScrRect();    
	AddChild(m_pLSV = new iLanguageSelectView(m_pMgr, this, AlignRect(iSize(320, 240), clRect, AlignCenter), 100));
}

iSize iDlg_GameLanguage::GetDlgMetrics() const
{
    return iSize(gApp.Surface().GetWidth(), gApp.Surface().GetHeight());
}

void iDlg_GameLanguage::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
    uint32 uid = pView->GetUID();
	if(uid == 100) {
		SetGameLanguage((GAME_LANG)param);
	}
    EndDialog(0);
}

void iDlg_GameLanguage::OnCompose()
{
	iRect rc = GetScrRect();
	sint16 sid = PDGG(HERO_DIALOG_BACK);
	gGfxMgr.Blit(sid, gApp.Surface(), AlignRect(gGfxMgr.Dimension(sid), rc, AlignCenter));
	iRect lsvrc = m_pLSV->GetRect();
/*	lsvrc.InflateRect(DLG_GLOW_FRAME_SIZE);
	ComposeGlowBkgnd(gApp.Surface(), lsvrc);
 */
}
