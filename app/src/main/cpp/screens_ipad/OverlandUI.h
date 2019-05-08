#pragma once

#include "GameViewInterface.h"

class iMinimapRawView;
class iArmyListEx_touch;

namespace UI_IPAD
{
    
    class iResourceBar: public IResourceBar
    {
    public:
        iResourceBar(iViewMgr* pViewMgr, uint32 uid);
        void SetMinerals(const iMineralSet* ms) { m_ms = ms; Invalidate(); }
        void OnCompose();
        iSize GetDefSize();
        void AnimateResources(const iMineralSet& was, const iMineralSet& now);
    private:
        const iMineralSet* m_ms;
        bool  m_bAnimating;
        iMineralSet m_currentMinSet;
    };
    
    class iOvalButton: public iButton
    {
    public:
        iOvalButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, iRect rc, uint32 uid);
        inline void SetHero(iHero *pHero) { m_pHero = pHero; Invalidate(); }
        inline void SetCastle(iCastle *pCastle) { m_pCastle = pCastle; Invalidate(); }
        void OnCompose();
        void SetHighlighted( bool flag );
        
        void OnMouseEntered(const iPoint& pos, MouseId mID){
            
            m_bHighlighted =  true;
        }
        void OnMouseLeave(const iPoint& pos, MouseId mID){
            
            m_bHighlighted = false;
        }
    private:
        iHero* m_pHero;
        iCastle* m_pCastle;
        bool m_bHighlighted;
    };
    
    class iOverlandToolbar: public IOverlandToolbar
    {
    public:
        iOverlandToolbar(iViewMgr* pViewMgr, uint32 uid);
        void OnCompose();
        void AfterCompose();
        void UpdateSize();
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        
        iSize GetDefSize() const { return iSize(1024, 130); }
        void SetHero(iHero* pHero);
        void SetCastle(iCastle* pCastle);
        void SetMapPos(iPoint pos) { m_pos = pos; }
        void ComposeInfoBar();
        void UpdateMinimap();
        void CenterMinimap();
        void AnimateExp(sint32 oldperc, sint32 newperc);
        void FinishAnimatingExp();
        void SetInfoMode(bool bInfoMode);
        //void BlinkArmyCell(sint32 idx);
        void SetHighlighted( bool flag ){ m_pHeroBtn->SetHighlighted(flag); }
        
        void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        void OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        void OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        void OnMouseEntered(const iPoint& pos, MouseId mID){}
        void OnMouseLeave(const iPoint& pos, MouseId mID){}
    private:
        iMinimapRawView *m_pMinimap;
        iOvalButton* m_pHeroBtn;
        iPoint		m_pos;
        iCheckButton *m_pSplitBtn;
        iArmyListEx_touch *m_pArmyList;
        iHero*	m_pHero;
        iCastle* m_pCastle;
        sint32	m_expPerc;
        sint16	m_expBarSprite;
    };
    
}
