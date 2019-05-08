#ifndef PHEROES_GAME_DLG_MESSAGE_H_
#define PHEROES_GAME_DLG_MESSAGE_H_

class iDlg_Message : public iTextDlg
{
public:
	iDlg_Message(iViewMgr* pViewMgr, iHero* pHero, iMapItem* pMapItem);

	// inlines
	inline iMapItem* MapItem() { return m_pMapItem; }
	inline iHero* Hero() { return m_pHero; }

private:
	iMapItem*		m_pMapItem;
	iHero*			m_pHero;
};


#endif //PHEROES_GAME_DLG_MESSAGE_H_
