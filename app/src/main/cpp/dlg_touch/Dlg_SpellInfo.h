#ifndef PHEROES_GAME_DLG_SPELLINFO_H_
#define PHEROES_GAME_DLG_SPELLINFO_H_

class iSpellInfoDlg : public iTextDlg
{
public:
	iSpellInfoDlg(iViewMgr* pViewMgr, MAGIC_SPELL spell, MAGIC_SCHOOL_LEVEL level, PLAYER_ID pid);

protected:
	void DoCompose(const iRect& clRect);
	iSize ClientSize() const;

protected:
	MAGIC_SPELL			m_spell;
};


#endif //PHEROES_GAME_DLG_SPELLINFO_H_
