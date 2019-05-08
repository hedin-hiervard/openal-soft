#pragma once

#include "GameViewInterface.h"

class iArmyExchange;
class iArmyListEx_touch;
class iMarketView;
class iMGuildView;
class iDockView;
class iRecruitHeroView;
class iCreatDescView;
class iDwellCtlCnst;
class iTavernDockView;
class iCreatureRecruitView;
class iScreenSlider;

namespace UI_IPHONE
{
    class iRecrBtn;
    
    class iCreatureRecruitView : public iView, public IViewCmdHandler
    {
    public:
        iCreatureRecruitView(iViewMgr* pViewMgr, IViewCmdHandler *pCmdHandler, uint32 uid);
        void UpdateSize();
        void OnActivate();
        void OnCompose();
        void SetCastle(iCastle* pCastle);
        void Update();
        void SetButtonSelected(sint32 idx);
        void SetCreatureTypeSelected(CREATURE_TYPE ct);
        CREATURE_TYPE GetSelectedCreatureType() const;
    protected:
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        void RecruitCreatures(uint32 count);
        
    protected:
        iCastle*		m_pCastle;
        iAmountChooserGlow* m_pAmount;
        iButton*		m_pRecruitBtn;  ///< The pointer to recruit button
        iButton*		m_pBuyGoldBtn;
        iButton*        m_pInfoBtn;
        iRecrBtn*		m_pCrBtns[6];
        iArmyExchange*	m_pArmyExchange;
        IViewCmdHandler* m_pCmdHandler; ///< The pointer to command handler
        iCheckButton*	m_pSplitBtn;
    };
    
    class iKingdomOverviewView : public iView, public IViewCmdHandler
    {
    public:
        iKingdomOverviewView(iViewMgr* pViewMgr, IViewCmdHandler *pCmdHandler, uint32 uid);
        void OnCompose();
    protected:
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        
    protected:
        IViewCmdHandler* m_pCmdHandler; ///< The pointer to command handler
    };
    
    
    
    class iBuildView: public iView, public IViewCmdHandler {
    public:
        struct bldinfo {
            sint32 bldidx;
            iPoint center;
            iRect rc;
            iStringT text;
            iSize costsz;
            iMineralSet cost;
            CREATURE_TYPE crtype;
            bool available;
        };
        
        enum CastleViewMode {
            CVM_MAIN = 0,
            CVM_EMPTY, // for modal dialogs
            CVM_BUILD
        };
        
        struct iCFSAni {
            enum Type {
                CFSA_BLD_ALPHA
            };
            
            iCFSAni(Type type, fix32 length, sint32 fparam, sint32 sparam): m_type(type), m_length(length), m_fparam(fparam), m_sparam(sparam), m_timer(fix32::zero) {}
            bool Process(fix32 t) {
                m_timer += t;
                if(m_type == CFSA_BLD_ALPHA)
                    m_sparam = (uint8)(255 * m_timer / m_length).floor();
                return m_timer >= m_length;
            }
            Type m_type;	///< The type
            fix32 m_length; ///< The length
            fix32 m_timer;  ///< The timer
            sint32 m_fparam;	///< The first parameter
            sint32 m_sparam;	///< The second parameter
        };
    public:
        iBuildView(iViewMgr* pViewMgr, iRect rc, uint32 uid);
        
        
        sint32 GetBuildingClicked( const iPoint& pos, bool bBuilt );
        sint32 GetBldInfoClicked( const iPoint& pos);
        sint16 GetCtlSprite( CTL_CONSTR cnst, uint32& idx );
        sint32 SelectBuiltMGuild( sint32 clicked );
        
        void SetCastle(iCastle* pCastle);
        void SetViewMode(CastleViewMode mode);
        void OnCompose();
        void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        bool Process(fix32 t);
        void Update();
        void UpdateSize();
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        
        void OnModalDlgAdded();
        void OnModalDlgRemoved();
        inline void SetBackgroundMode(bool bBack) { m_bBackgroundMode = bBack; }
    private:
        CTL_CONSTR			m_selectedCnst; ///< The selected construction
        iCFSAni				*m_pAni;	///< The pointer to animation
        CastleViewMode		m_prevViewMode;
        iSimpleArray<bldinfo> m_bldinfolist;
        CastleViewMode		m_viewMode; ///< The view mode
        iImgBtn*			m_pBuildBtn; ///< Castle building button on the (mid-left)
        iCastle*			m_pCastle;
        bool				m_bBackgroundMode;
    };
    
    ///-------------------------------------------------------------------------------------------------
    /// Tavern and dock view.
    ///-------------------------------------------------------------------------------------------------
    
    class iTavernDockView: public iView, public IViewCmdHandler {
    public:
        
        ///-------------------------------------------------------------------------------------------------
        /// Constructor.
        ///
        /// \param [in,out]	pViewMgr	If non-null, pointer to view manager.
        /// \param [in,out]	pHandler	If non-null, the pointer to handler.
        /// \param [in,out]	rc			The rectangle.
        /// \param	uid					The uid.
        ///-------------------------------------------------------------------------------------------------
        
        iTavernDockView(iViewMgr* pViewMgr, IViewCmdHandler *pHandler, const iRect& rc, uint32 uid);
        
        ///-------------------------------------------------------------------------------------------------
        /// Sets a castle.
        ///
        /// \param [in,out]	pCastle	If non-null, the pointer to castle.
        ///-------------------------------------------------------------------------------------------------
        
        void SetCastle(iCastle *pCastle);
        
        ///-------------------------------------------------------------------------------------------------
        /// Executes the compose action.
        ///-------------------------------------------------------------------------------------------------
        
        void OnCompose();
        
        ///-------------------------------------------------------------------------------------------------
        /// Updates the size.
        ///-------------------------------------------------------------------------------------------------
        
        void UpdateSize();
        
        ///-------------------------------------------------------------------------------------------------
        /// Command handler.
        ///
        /// \param [in,out]	pView	If non-null, the pointer to view.
        /// \param	cmd				The command.
        /// \param	param			The parameter.
        ///-------------------------------------------------------------------------------------------------
        
        void iCMDH_ControlCommand(iView *pView, CTRL_CMD_ID cmd, sint32 param);
        
        ///-------------------------------------------------------------------------------------------------
        /// Updates this object.
        ///-------------------------------------------------------------------------------------------------
        
        void Update();
        
        void SetHighlighted( bool flag );
        
    public:
        static const iSize sm_defSize;
    private:
        iDockView		*m_pDockView;   ///< The pointer to dock view
        iRecruitHeroView	*m_pRecrHeroView;   ///< The pointer to recr hero view
        IViewCmdHandler		*m_pHandler;	///< The pointer to handler
        iCastle*			m_pCastle;  ///< The pointer to castle
    };
    
    ///-------------------------------------------------------------------------------------------------
    /// Dialog build construction confirmation.
    ///-------------------------------------------------------------------------------------------------
    
    class iDlg_BuildCnstCnfrm: public iGlowGameDlg {
    public:
        
        ///-------------------------------------------------------------------------------------------------
        /// Constructor.
        ///
        /// \param [in,out]	pViewMgr	If non-null, pointer to view manager.
        /// \param [in,out]	pCnst		If non-null, the pointer to construction.
        ///-------------------------------------------------------------------------------------------------
        
        iDlg_BuildCnstCnfrm(iViewMgr* pViewMgr, iCtlCnst* pCnst);
#if defined( PC_VERSION )
        void OnClickNo() { EndDialog(DRC_CANCEL); }
        void OnClickYes() { EndDialog(DRC_OK); }
        void OnDown() {}
#endif
    private:
        
        ///-------------------------------------------------------------------------------------------------
        /// Executes the create dialog action.
        ///-------------------------------------------------------------------------------------------------
        
        void OnCreateDlg();
        
        ///-------------------------------------------------------------------------------------------------
        /// Executes the compose operation.
        ///
        /// \param	rect	The rectangle.
        ///-------------------------------------------------------------------------------------------------
        
        void DoCompose(const iRect& rect);
        
        ///-------------------------------------------------------------------------------------------------
        /// Gets the client size.
        ///
        /// \return	.
        ///-------------------------------------------------------------------------------------------------
        
        iSize ClientSize() const;
        
        ///-------------------------------------------------------------------------------------------------
        /// Command handler.
        ///
        /// \param [in,out]	pView	If non-null, the pointer to view.
        /// \param	cmd				The command.
        /// \param	param			The parameter.
        ///-------------------------------------------------------------------------------------------------
        
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        
        ///-------------------------------------------------------------------------------------------------
        /// Updates the size.
        ///-------------------------------------------------------------------------------------------------
        void UpdateSize();
        
    private:
        iCtlCnst* m_pCnst;  ///< The pointer to construction
        iImgBtn*            m_pBtnBuild;
        iImgBtn*            m_pBtnCancel;
    };
    
    
    ///-------------------------------------------------------------------------------------------------
    /// New castle view.
    ///
    /// \author	hedin
    /// \date	13.02.2011
    ///-------------------------------------------------------------------------------------------------
    
    class iCastleView : public ICastleView
    {
    public:
        
    public:
        ///-------------------------------------------------------------------------------------------------
        /// Default constructor.
        ///-------------------------------------------------------------------------------------------------
        
        iCastleView();
        
        ///-------------------------------------------------------------------------------------------------
        /// Destructor.
        ///
        ///-------------------------------------------------------------------------------------------------
        
        ~iCastleView();
        
        ///-------------------------------------------------------------------------------------------------
        /// Sets a castle.
        ///
        /// \param [in,out]	pCastle	If non-null, the pointer to castle.
        ///-------------------------------------------------------------------------------------------------
        
        void SetCastle(iCastle* pCastle);
        
        bool Process(fix32 t);
        
        void OnTripleSwipeRight(sint32 length);
        void OnTripleSwipeLeft(sint32 length);
        ///-------------------------------------------------------------------------------------------------
        /// Executes the swipe up action.
        ///
        /// \param	length	The length.
        ///-------------------------------------------------------------------------------------------------
        
        void OnSwipeUp(sint32 length);
        void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        
        virtual inline bool AlwaysReceiveMouse() { return true; }
        
        void OnActivate(bool bActivate);
        
        virtual CHILD_VIEW Type() const { return CV_CASTLE; }
        
        virtual iSize GetDefaultSize() const { return gApp.ViewMgr().Metrics(); }
        
        ///-------------------------------------------------------------------------------------------------
        /// Updates this object.
        ///-------------------------------------------------------------------------------------------------
        
        void Update();
        
        
        void OnCompose();
        
        void SetScreen(CastleScreen scr);
        void SetRecruitCreatureType(CREATURE_TYPE ct);
        
    public:
        
    private:
        
        ///-------------------------------------------------------------------------------------------------
        /// Updates the size.
        ///-------------------------------------------------------------------------------------------------
        
        void UpdateSize();
        
        ///-------------------------------------------------------------------------------------------------
        /// Command handler 
        ///
        /// \param [in,out]	pView	If non-null, the pointer to view. 
        /// \param	cmd				The command. 
        /// \param	param			The parameter. 
        ///-------------------------------------------------------------------------------------------------
        
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        
        ///-------------------------------------------------------------------------------------------------
        /// Gets a castle sprite. 
        ///
        /// \param	cnst		The construction. 
        /// \param [in,out]	idx	Zero-based index of the castle sprite in the z-order list. 
        ///
        /// \return	The castle sprite. 
        ///-------------------------------------------------------------------------------------------------
        
        sint16 GetCtlSprite( CTL_CONSTR cnst, uint32& idx );	
        
        virtual void OnModalDlgAdded();
        virtual void OnModalDlgRemoved();
        
        private	:	
        bool				m_bGestureComplete; ///< true to b gesture complete
        iCastle*			m_pCastle;	///< Current shown castle	
        iImgBtn*			m_pMagicGuildBtn;   ///< The pointer to magic guild button
        iImgBtn*			m_pRecrBtn; ///< Creature recruit button (top-right)
        iImgBtn*			m_pMarketBtn;  ///< Market button (bot-left)
        iImgBtn*			m_pTavernDockBtn; ///< Tavern & dock button (bot-right)
        iImgBtn*			m_pExitBtn; ///< Tavern & dock button (bot-right)
        iArmyExchange*		m_pArmyExchange;	///< The pointer army exchange
        
        iBuildView			*m_pBuildView;
        iMarketView			*m_pMarketView; ///< The pointer to market view
        iMGuildView			*m_pMGuildView; ///< The pointer to magic guild view
        iTavernDockView		*m_pTDView;  ///<Tavern & Dock view	
        iKingdomOverviewView *m_pKOView;
        
        iCreatureRecruitView *m_pRecrView;  ///< The pointer recr view
        iButton				*m_pPrevCastleBtn;  ///< The pointer previous castle button
        iButton				*m_pNextCastleBtn;  ///< The pointer next castle button
        iButton*			m_pDismissBtn;  ///< The pointer to dismiss button
        
        
        iScreenSlider*		m_pScreenSlider;
        uint32 BUILD_SLIDE, RECRUIT_SLIDE, MARKET_SLIDE, MGUILD_SLIDE, TAVERN_SLIDE, KO_SLIDE;
        bool				m_bFalling;
        
    };
    
}
