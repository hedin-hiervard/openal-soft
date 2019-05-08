#include "stdafx.h"
#include "Dlg_SpellInfo.h"

iSpellInfoDlg::iSpellInfoDlg(iViewMgr* pViewMgr, MAGIC_SPELL spell, MAGIC_SCHOOL_LEVEL level, PLAYER_ID pid)
: iTextDlg(pViewMgr, gTextMgr[gGame.ItemMgr().m_spellMgr.Spell(spell)->NameKey()], gTextMgr[gGame.ItemMgr().m_spellMgr.Spell(spell)->DescKey(level)], pid), m_spell(spell)
{}

void iSpellInfoDlg::DoCompose(const iRect& clRect)
{
	iRect rc(clRect);
	iTextDlg::DoCompose(clRect);
	iPoint op(rc.x + (rc.w/2 - 30), rc.y2() - 15 - 10 - 26);
	gGfxMgr.Blit(PDGG(SPELLSCROLL_UNROLLED),gApp.Surface(),op);
	op.Move(15,7);
	gGfxMgr.Blit(gGame.ItemMgr().m_spellMgr.Spell(m_spell)->Icon(),gApp.Surface(),op);
}

iSize iSpellInfoDlg::ClientSize() const
{
	iSize res = iTextDlg::ClientSize();
	res.h += 46;
    res.w = iMIN<sint32>(gApp.Surface().GetWidth(), res.w);
    res.h = iMIN<sint32>(gApp.Surface().GetHeight(), res.h);
    
	return res;
}


