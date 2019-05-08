#ifndef PHEROES_GAME_HERO_ARTIFACTS_STYLUS_H_
#define PHEROES_GAME_HERO_ARTIFACTS_STYLUS_H_

//////////////////////////////////////////////////////////////////////////
class iArtifactsTab_stylus : public iHeroViewChild, public IViewCmdHandler
{
public:
	iArtifactsTab_stylus(iViewMgr* pViewMgr);
	void OnHeroChanged();
	void ComposeArtCell(const iRect& rc, uint16 artId);	
	void OnCompose();
	
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);	
private:
	sint32 CellByPos(const iPoint& pos);	
	void OnMouseDown(const iPoint& pos, MouseId mID);	
	void OnMouseUp(const iPoint& pos);	
private:
	iSimpleArray<iArtDragDropItem*>		m_Competitors;
	iArtItemCtrl*						m_pDollItems[AC_COUNT];
	iArtBackPackCtrl*					m_pArtBackPackCtrl;
	iSimpleArray<iRect>					m_dollCells;
};


#endif //PHEROES_GAME_HERO_ARTIFACTS_STYLUS_H_
