#include "stdafx.h"
#include "Dlg_Spellbook.h"

#ifdef IPHONE_DEBUG
#define DEBUG
#endif

const uint32 SPELL_SET_MASKS[SSM_COUNT - 1] = {
MST_MASK_ALL,
MST_MASK_AIR,
MST_MASK_EARTH,
MST_MASK_FIRE,
MST_MASK_WATER
};

#if 0
const iRect iSpellBookView::sm_area_air = iRect(58, 165, 28, 35),
iSpellBookView::sm_area_fire = iRect(141, 165, 35, 35),
iSpellBookView::sm_area_earth = iRect(86, 165, 55, 35),
iSpellBookView::sm_area_water = iRect(176, 165, 30, 35),
iSpellBookView::sm_area_all = iRect(206, 165, 50, 35),
iSpellBookView::sm_area_prev = iRect(0, 0, 35, 35),
iSpellBookView::sm_area_next = iRect(180, 0, 35, 35);
#else
const iRect iSpellBookView::sm_area_air = iRect(178, 227, 28, 35),
iSpellBookView::sm_area_fire = iRect(261, 227, 35, 35),
iSpellBookView::sm_area_earth = iRect(206, 227, 55, 35),
iSpellBookView::sm_area_water = iRect(296, 227, 30, 35),
iSpellBookView::sm_area_all = iRect(326, 227, 50, 35),
iSpellBookView::sm_area_prev = iRect(0, 0, 35, 35),
iSpellBookView::sm_area_next = iRect(300, 0, 35, 35);
#endif

iSpellBookView::iSpellBookView(iViewMgr* pViewMgr, IViewCmdHandler* cmdHandler, const iRect& rect, uint32 uid, iHero* owner, uint32 typeMask, SPELL_SET_MODES ssm,
uint32 startingPage, bool bLandOnly)
: iView(pViewMgr, rect, GENERIC_VIEWPORT, uid, Visible|Enabled), m_bGestureComplete(false),
							   m_pCmdHandler(cmdHandler), m_ssm(ssm), m_typeMask(typeMask),
								m_page(startingPage), m_pOwner(owner), m_bLandOnly(bLandOnly)
{
	for(uint32 x=0; x<4; x++) {
        iRect rc(80 + 170 * (x / 2), 20 + 100 * (x % 2), 130, 80);		
		AddChild(m_items[x] = new iSpellBookItem(pViewMgr, this, rc, 100 + x, m_pOwner)); 
	}	

	AddChild(m_pPrevPageBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 114, PDGG(BTN_SWITCH_CASTLES_DELUXE), PDGG(BTN_SWITCH_CASTLES_DELUXE) + 2));
	AddChild(m_pNextPageBtn = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 115, PDGG(BTN_SWITCH_CASTLES_DELUXE) + 1, PDGG(BTN_SWITCH_CASTLES_DELUXE) + 3));
}

void iSpellBookView::UpdateSize()
{
	m_pPrevPageBtn->SetRect(AlignChild(iSize(50, 50), AlignLeft));
	m_pNextPageBtn->SetRect(AlignChild(iSize(50, 50), AlignRight));
		
	
	for(uint32 x=0; x<4; x++) {
		iRect rc(80 + 170 * (x / 2), 20 + 100 * (x % 2), 130, 80);		
		m_items[x]->SetRect(rc);
	}	
}

void iSpellBookView::SetOwner(iHero *owner) 
{
	m_pOwner = owner;
	m_page = 0;
	for(uint32 x=0; x<4; x++) {
		m_items[x]->SetOwner(owner);
	}		
	BuildSpellList();
	UpdateSpells();
}

void iSpellBookView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	for(uint32 x=0; x<4; x++)
		if(uid == 100 + x) {
			if(m_items[x]->Spell() != MSP_INVALID) {
				m_pCmdHandler->iCMDH_ControlCommand(this, cmd, m_items[x]->Spell());
			}
		}
	if(uid == 114)
		PrevPage();
	else if(uid == 115)
		NextPage();
}

void iSpellBookView::BuildSpellList()
{
	if (!m_pOwner) return;
	m_spells.Cleanup();
	/*if (m_ssm == SSM_FAVORITES) {
		for (uint32 xx=0; xx<m_pOwner->SpellBook().FavoritesCount(); ++xx) {
			if (m_typeMask & (1<<m_pOwner->SpellBook().FavoriteByIdx(xx)->Type())) {
				m_spells.Add(xx, xx);
			}
		}
	} else {*/
	uint32 spmask = SPELL_SET_MASKS[m_ssm];

	for (uint32 xx=0; xx<m_pOwner->SpellBook().SpellsCount(); ++xx) {
		if ((!m_bLandOnly || m_pOwner->SpellBook().SpellByIdx(xx)->Id() != MSP_TOWNPORTAL) && 
			(m_typeMask & (1<<m_pOwner->SpellBook().SpellByIdx(xx)->Type())) && (spmask & (1<<m_pOwner->SpellBook().SpellByIdx(xx)->School()))) {
			uint32 idx = m_pOwner->SpellBook().SpellByIdx(xx)->Level() * 100;
			idx += m_pOwner->SpellBook().SpellByIdx(xx)->School() * 10;
			if(m_pOwner->SpellBook().SpellByIdx(xx)->SpellClass() != SPC_DAMAGE)
				idx += 1;
			m_spells.Insert(m_pOwner->SpellBook().SpellByIdx(xx)->Id(), idx);
			//spells.Insert(m_pOwner->SpellBook().Spell(xx), SPELL_DESCRIPTORS[m_pOwner->SpellBook().Spell(xx)].level);
			//#ifndef DEBUG
		}
		//#endif
	}
}		

void iSpellBookView::PrevPage()
{
	if(m_page > 0) {
		m_page--;
		//m_pOwner->Owner()->ViewSettings().spellBookPage = m_page;
		UpdateSpells();
	}
}

void iSpellBookView::NextPage()
{
	if(m_page < m_spells.Size() / 4 - (1 - iMIN<uint32>(m_spells.Size() % 4, 1))) {
		m_page++;
		//m_pOwner->Owner()->ViewSettings().spellBookPage = m_page;
		UpdateSpells();
	}
}

void iSpellBookView::OnMouseDown(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	iRect rc = GetScrRect();
	
	m_bGestureComplete = false;	
/*	bool changed = false;
	if((sm_area_air + rc.point()).PtInRect(pos.x, pos.y)) { m_ssm = SSM_AIR; changed = true; }
	else if((sm_area_earth + rc.point()).PtInRect(pos.x, pos.y)) { m_ssm = SSM_EARTH; changed = true; }
	else if((sm_area_fire + rc.point()).PtInRect(pos.x, pos.y)) { m_ssm = SSM_FIRE; changed = true; }
	else if((sm_area_water + rc.point()).PtInRect(pos.x, pos.y)) { m_ssm = SSM_WATER; changed = true; }
	else if((sm_area_all + rc.point()).PtInRect(pos.x, pos.y)) { m_ssm = SSM_ALL; changed = true; }
	
	if(changed) {
		BuildSpellList();
		m_page = iMIN<uint32>(m_page, m_spells.Size() / 4 - (1 - iMIN<uint32>(m_spells.Size() % 4, 1)));
		//m_pOwner->Owner()->ViewSettings().spellBookPage = m_page;
		m_pOwner->Owner()->ViewSettings().spellbookSsm = m_ssm;
		UpdateSpells();
	}
*/
#ifndef OS_APPLE
	if((sm_area_prev + rc.point()).PtInRect(pos.x, pos.y)) PrevPage();
	if((sm_area_next + rc.point()).PtInRect(pos.x, pos.y)) NextPage();
#endif
}

void iSpellBookView::OnSwipeRight(sint32 length)
{
	if(!m_bGestureComplete && length > 30) {
		PrevPage();
		m_bGestureComplete = true;
	}
	//gApp.ViewMgr().SetViewCapture(this);

}

void iSpellBookView::OnSwipeLeft(sint32 length)
{
	if(!m_bGestureComplete && length > 30) {
		NextPage();
		m_bGestureComplete = true;
	}
}


void iSpellBookView::UpdateSpells()
{
	for(uint32 x=0; x<4; x++) {
		if(m_page * 4 + x < m_spells.Size())
			m_items[x]->SetSpell((MAGIC_SPELL)m_spells[m_page * 4 + x].value);
		else
			m_items[x]->SetSpell(MSP_INVALID);
	}
	
}

void iSpellBookView::OnCompose()
{
	if(!m_pOwner) return;
	iRect rc = GetScrRect() + iPoint(50, 0);
	iPoint offset = iPoint(0, 5);
	gGfxMgr.Blit(PDGG(SPELLBOOK_BACK), gApp.Surface(), rc);	
	iTextComposer::FontConfig fc(FS_PT_14, iDibFont::ComposeProps(RGB16(255, 255, 255),RGB16(48,32,0)));
	gTextComposer.TextOut(fc, gApp.Surface(), rc.point() + iPoint(26, 168 + 59), iFormat(_T("%d"), m_pOwner->ManaPts()));
/*	gGfxMgr.Blit(PDGG(SPELLBOOK_BOOKMARK_AIR), gApp.Surface(), offset + rc.point() + sm_area_air.point() + (m_ssm == SSM_AIR ? iPoint(0, -9) : iPoint(0, 0)));
	gGfxMgr.Blit(PDGG(SPELLBOOK_BOOKMARK_WATER), gApp.Surface(), offset + rc.point() + sm_area_water.point() + (m_ssm == SSM_WATER ? iPoint(0, -9) : iPoint(0, 0)));
	gGfxMgr.Blit(PDGG(SPELLBOOK_BOOKMARK_EARTH), gApp.Surface(), offset + rc.point() + sm_area_earth.point() + (m_ssm == SSM_EARTH ? iPoint(0, -9) : iPoint(0, 0)));
	gGfxMgr.Blit(PDGG(SPELLBOOK_BOOKMARK_FIRE), gApp.Surface(), offset + rc.point() + sm_area_fire.point() + (m_ssm == SSM_FIRE ? iPoint(0, -9) : iPoint(0, 0)));
	gGfxMgr.Blit(PDGG(SPELLBOOK_BOOKMARK_ALL), gApp.Surface(), offset + rc.point() + sm_area_all.point() + (m_ssm == SSM_ALL ? iPoint(0, -9) : iPoint(0, 0)));
 */
}

iSize iSpellBookView::sm_defSize = iSize(459, 264);


iSpellBookItem::iSpellBookItem(iViewMgr *pViewMgr, IViewCmdHandler* pHandler, iRect &rect, uint32 uid, iHero* pOwner)
: iButton(pViewMgr, pHandler, rect, uid, Visible|Enabled),  m_pOwner(pOwner), m_spell(MSP_INVALID)
{
	
}

void iSpellBookItem::OnCompose()
{
	if(m_spell == MSP_INVALID) 
		return;

	iRect rc = GetScrRect();	
		
	sint16 sid = gGame.ItemMgr().m_spellMgr.Spell(m_spell)->Icon();
	gGfxMgr.BlitEffect(sid, gApp.Surface(), AlignRect(gGfxMgr.Dimension(sid), rc, AlignTop), (m_state & Pressed) ? iGfxManager::EfxGlow : iGfxManager::EfxNone);
	
	rc.y += gGfxMgr.Dimension(sid).h + 5;

	iTextComposer::FontConfig fc = spellbookfc;
	fc.cmpProps = iDibFont::ComposeProps(MSCH_COLORS[SPELL_DESCRIPTORS[m_spell].school][1], MSCH_COLORS[SPELL_DESCRIPTORS[m_spell].school][1], iDibFont::DecNone);
	rc.y += gTextComposer.TextBoxOut(fc, gApp.Surface(),gTextMgr[TRID_SPNAME_MAGICARROW+m_spell],rc);
	iStringT text(gTextMgr[TRID_MANA]);
	uint8 cost = iBaseSpell::GetSpellCost(m_spell, m_pOwner);
	text.Addf(_T(": %d"), cost);	
	fc.cmpProps = iDibFont::ComposeProps(MSCH_COLORS[SPELL_DESCRIPTORS[m_spell].school][1],MSCH_COLORS[SPELL_DESCRIPTORS[m_spell].school][1],iDibFont::DecNone);
	rc.y += gTextComposer.TextBoxOut(fc, gApp.Surface(), text,rc);
    
    iBaseSpell *pSpell = gGame.ItemMgr().m_spellMgr.Spell(m_spell);
    if (pSpell->SpellClass() == SPC_DAMAGE)
    {
        iSpell_Damage* pDmgSpell = (iSpell_Damage*)pSpell;

        text = iFormat(gTextMgr[TRID_MSG_BAT_DAMAGE1], pDmgSpell->SpellBookDamage(m_pOwner));
        gTextComposer.GetFontHeight(fc.fontSize);;
		fc.cmpProps = iDibFont::ComposeProps(MSCH_COLORS[SPELL_DESCRIPTORS[m_spell].school][1],MSCH_COLORS[SPELL_DESCRIPTORS[m_spell].school][1],iDibFont::DecNone);
        rc.y += gTextComposer.TextBoxOut(fc, gApp.Surface(),text,rc);
    }		

}

iDlg_Spellbook::iDlg_Spellbook(iViewMgr* pViewMgr, iHero* pOwner, uint32 typeMask, bool bLandOnly):
iDialog(pViewMgr), m_pOwner(pOwner), m_selItem(MSP_INVALID)
{
    // old size was iSize(239, 202) 
	AddChild(m_spellbook = new iSpellBookView(pViewMgr, this, iRect(GetScrPos(), iSpellBookView::sm_defSize), 100, pOwner,  typeMask, (SPELL_SET_MODES)pOwner->Owner()->ViewSettings().spellbookSsm, 0, bLandOnly));	
	
	m_spellbook->BuildSpellList();
	m_spellbook->UpdateSpells();
	
}

void iDlg_Spellbook::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	iRect rc = GetScrRect();
	if(!rc.PtInRect(pos.x, pos.y))
		EndDialog(DRC_CANCEL);
}

void iDlg_Spellbook::OnCreateDlg()
{
//#ifdef PC_VERSION
	m_spellbook->UpdateSize();
//#endif
}

void iDlg_Spellbook::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if(uid == 100) {
		m_selItem = (MAGIC_SPELL)param;
		if(CanCastSelSpell())
			EndDialog(DRC_YES);
		else {
			iTextDlg dlg(&gApp.ViewMgr(), _T(""), gTextMgr[TRID_MSG_SPELL_NOT_ENOUGH_MANA], m_pOwner->Owner()->PlayerId());
			dlg.DoModal();
		}
	} else if(uid == 116) {
		EndDialog(DRC_NO);
	}
}

iSize iDlg_Spellbook::ClientSize() const
{
	return iSpellBookView::sm_defSize;
}

iSize iDlg_Spellbook::GetDlgMetrics() const
{
	return iSpellBookView::sm_defSize;
}

void iDlg_Spellbook::OnCompose()
{
	gApp.Surface().Darken50Rect(iRect(0, 0, gApp.Surface().GetWidth(), gApp.Surface().GetHeight()));
	gApp.Surface().Darken50Rect(iRect(0, 0, gApp.Surface().GetWidth(), gApp.Surface().GetHeight()));
}

MAGIC_SPELL iDlg_Spellbook::SelSpell() const
{
	return m_selItem;
}