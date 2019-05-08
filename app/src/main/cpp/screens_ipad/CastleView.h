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
class iRecrSet;

namespace UI_IPAD
{
    
    
    ///-------------------------------------------------------------------------------------------------
    /// Creature recruit view.
    ///-------------------------------------------------------------------------------------------------
    
    class iCreatureRecruitView : public iView, public IViewCmdHandler
    {
    public:
        
        ///-------------------------------------------------------------------------------------------------
        /// Constructor.
        ///
        /// \param [in,out]	pViewMgr	If non-null, pointer to view manager.
        /// \param [in,out]	pCmdHandler	If non-null, the pointer to command handler.
        /// \param	uid					The uid.
        ///-------------------------------------------------------------------------------------------------
        
        iCreatureRecruitView(iViewMgr* pViewMgr, IViewCmdHandler *pCmdHandler, uint32 uid);
        
        ///-------------------------------------------------------------------------------------------------
        /// Updates the size.
        ///-------------------------------------------------------------------------------------------------
        
        void UpdateSize();
        
        ///-------------------------------------------------------------------------------------------------
        /// Executes the activate action.
        ///-------------------------------------------------------------------------------------------------
        
        void OnActivate();
        
        ///-------------------------------------------------------------------------------------------------
        /// Executes the compose action.
        ///-------------------------------------------------------------------------------------------------
        
        void OnCompose();
        
        ///-------------------------------------------------------------------------------------------------
        /// Sets a castle.
        ///
        /// \param [in,out]	pCastle	If non-null, the pointer to castle.
        ///-------------------------------------------------------------------------------------------------
        
        void SetCastle(iCastle* pCastle);
        
        ///-------------------------------------------------------------------------------------------------
        /// Gets the size.
        ///
        /// \return	The size.
        ///-------------------------------------------------------------------------------------------------
        
        iSize GetSize();
        
        ///-------------------------------------------------------------------------------------------------
        /// Updates this object.
        ///-------------------------------------------------------------------------------------------------
        
        void Update();
        
        ///-------------------------------------------------------------------------------------------------
        /// Sets a selected creature type.
        ///
        /// \param	ct	Type of the creature.
        ///-------------------------------------------------------------------------------------------------
        
        void SetSelectedCT(CREATURE_TYPE ct);
        
        void OnMouseWheelDown(sint16 line, MouseId mID);
        
        bool IsScrolling() {return true;}
        
    protected:
        
        ///-------------------------------------------------------------------------------------------------
        /// Command handler.
        ///
        /// \param [in,out]	pView	If non-null, the pointer to view.
        /// \param	cmd				The command.
        /// \param	param			The parameter.
        ///-------------------------------------------------------------------------------------------------
        
        void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
        
        ///-------------------------------------------------------------------------------------------------
        /// Recruit creatures.
        ///
        /// \param	count	Number of.
        ///-------------------------------------------------------------------------------------------------
        
        void RecruitCreatures(uint32 count);
        
    protected:
        iCastle*		m_pCastle;
        iRecrSet		*m_pRecrSet;
        iAmountChooserGlow* m_pAmount;
        iCreatDescView*  m_pDescription;	///< The pointer to creature description view
        iImgBtn*		m_pRecruitBtn;  ///< The pointer to recruit button
        iImgBtn*		m_pBuyGoldBtn;  ///< The pointer to recruit button
        iButton*        m_pInfoBtn;
        IViewCmdHandler *m_pCmdHandler; ///< The pointer to command handler
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
        
        struct bldinfo {
            sint32 bldidx;
            iPoint center;
            iRect rc;
            iStringT text;
            iSize costsz;
            iMineralSet cost;
            CREATURE_TYPE crtype;
        };
        
        
        ///-------------------------------------------------------------------------------------------------
        /// Castle FullScreen animation.
        ///-------------------------------------------------------------------------------------------------
        
        struct iCFSAni {
            enum Type {
                CFSA_BLD_ALPHA
            };
            
            ///-------------------------------------------------------------------------------------------------
            /// Constructor.
            ///
            /// \param	type	The type.
            /// \param	length	The length.
            /// \param	fparam	The first param.
            /// \param	sparam	The second param.
            ///-------------------------------------------------------------------------------------------------
            
            iCFSAni(Type type, fix32 length, sint32 fparam, sint32 sparam): m_type(type), m_length(length), m_fparam(fparam), m_sparam(sparam), m_timer(fix32::zero) {}
            
            ///-------------------------------------------------------------------------------------------------
            /// Process the given time interval.
            ///
            /// \param	t	The time interval.
            ///
            /// \return	true if it succeeds, false if it fails.
            ///-------------------------------------------------------------------------------------------------
            
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
        
        ///-------------------------------------------------------------------------------------------------
        /// Values that represent CastleViewMode.
        ///-------------------------------------------------------------------------------------------------
        
        enum CastleViewMode {
            CVM_MAIN = 0,
            CVM_EMPTY, // for modal dialogs
            CVM_MARKET,
            CVM_TAVERNDOCK,
            CVM_RECRUIT,
            CVM_MGUILD,
            CVM_CREAT_DETAILS,
            CVM_BUILD
        };
        
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
        
        ///-------------------------------------------------------------------------------------------------
        /// Executes the compose action.
        ///-------------------------------------------------------------------------------------------------
        
        void DoCompose();
        
        
        void OnTripleSwipeRight(sint32 length);
        void OnTripleSwipeLeft(sint32 length);
        ///-------------------------------------------------------------------------------------------------
        /// Executes the swipe up action.
        ///
        /// \param	length	The length.
        ///-------------------------------------------------------------------------------------------------
        
        void OnSwipeUp(sint32 length);
        
        ///-------------------------------------------------------------------------------------------------
        /// Executes the mouse down action.
        ///
        /// \param	pos	The position.
        /// \param	mID	The identifier.
        ///-------------------------------------------------------------------------------------------------
        
        void OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        
        ///-------------------------------------------------------------------------------------------------
        /// Gets a building clicked.
        ///
        /// \param	pos	The position.
        ///
        /// \return	The building clicked.
        ///-------------------------------------------------------------------------------------------------
        
        sint32 GetBuildingClicked( const iPoint& pos, bool bBuilt );
        
        
        sint32 GetBldInfoClicked( const iPoint& pos);
        
        ///-------------------------------------------------------------------------------------------------
        /// Executes the mouse up action.
        ///
        /// \param	pos	The position.
        /// \param	mID	The mouse identifier.
        ///-------------------------------------------------------------------------------------------------
        
        void OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        
        ///-------------------------------------------------------------------------------------------------
        /// Executes the mouse click action.
        ///
        /// \param	pos	The position.
        /// \param	mID	The mouse identifier.
        ///-------------------------------------------------------------------------------------------------
        
        void OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        
        void OnMouseDoubleClick(const iPoint& pos, MouseId mID, MouseButtonId mbID);
        ///-------------------------------------------------------------------------------------------------
        /// Gets the always receive mouse.
        ///
        /// \return	true if it succeeds, false if it fails.
        ///-------------------------------------------------------------------------------------------------
        
        virtual inline bool AlwaysReceiveMouse() { return true; }
        
        ///-------------------------------------------------------------------------------------------------
        /// Executes the activate action.
        ///
        /// \param	bActivate	true if activating.
        ///-------------------------------------------------------------------------------------------------
        
        void OnActivate(bool bActivate);
        
        virtual CHILD_VIEW Type() const { return CV_CASTLE; }
        
        //virtual iSize GetDefaultSize() const { return gApp.ViewMgr().Metrics(); }
        virtual iSize GetDefaultSize() const { return iSize(iMIN<sint32>(1024, gApp.ViewMgr().Metrics().w), iMIN<sint32>(768, gApp.ViewMgr().Metrics().h)); }
        
        ///-------------------------------------------------------------------------------------------------
        /// Sets a view mode.
        ///
        /// \param	mode	The mode.
        ///-------------------------------------------------------------------------------------------------
        
        void SetViewMode(CastleViewMode mode);
        
        ///-------------------------------------------------------------------------------------------------
        /// Updates this object.
        ///-------------------------------------------------------------------------------------------------
        
        void Update();
        void SetHighlighted( iCastleView::CastleViewMode cm );
        
        void OnMouseWheelDown(sint16 line, MouseId mID){
            
            m_pRecrView->OnMouseWheelDown( line, mID );
        }
        
        inline CastleViewMode GetViewMode(){ return m_viewMode; }
        
    public:
        static const sint32 sm_defSubviewWidth;
        static const iSize sm_defSubviewSize;
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
        
        
        ///-------------------------------------------------------------------------------------------------
        /// Process the given time interval. 
        ///
        /// \param	t	The time interval. 
        ///
        /// \return	true if it succeeds, false if it fails. 
        ///-------------------------------------------------------------------------------------------------
        
        bool Process(fix32 t);
        
        virtual void OnModalDlgAdded();
        virtual void OnModalDlgRemoved();
        sint32 SelectBuiltMGuild( sint32 clicked );
        private	:	
        bool				m_bGestureComplete; ///< true to b gesture complete
        iCastle*			m_pCastle;	///< Current shown castle	
        iImgBtn*			m_pBuildBtn; ///< Castle building button on the (mid-left)
        iImgBtn*			m_pMagicGuildBtn;   ///< The pointer to magic guild button
        iImgBtn*			m_pRecrBtn; ///< Creature recruit button (top-right)
        iImgBtn*			m_pMarketBtn;  ///< Market button (bot-left)
        iImgBtn*			m_pTavernDockBtn; ///< Tavern & dock button (bot-right)
        iImgBtn*			m_pExitBtn; ///< Tavern & dock button (bot-right)
        iArmyExchange*		m_pArmyExchange;	///< The pointer army exchange
        
        CastleViewMode		m_viewMode; ///< The view mode
        iMarketView			*m_pMarketView; ///< The pointer to market view
        iMGuildView			*m_pMGuildView; ///< The pointer to magic guild view
        iTavernDockView		*m_pTDView;  ///<Tavern & Dock view	
        iCreatureRecruitView *m_pRecrView;  ///< The pointer recr view
        iButton				*m_pPrevCastleBtn;  ///< The pointer previous castle button
        iButton				*m_pNextCastleBtn;  ///< The pointer next castle button
        CTL_CONSTR			m_selectedCnst; ///< The selected construction
        iCFSAni				*m_pAni;	///< The pointer to animation
#ifndef PC_VERSION
        iButton*			m_pDismissBtn;  ///< The pointer to dismiss button
#endif
        
        CastleViewMode		m_prevViewMode;
        
        iSimpleArray<bldinfo> m_bldinfolist;
#if defined PC_VERSION
    public:
        bool                m_bMarket;
        bool                m_bTavern;
        bool                m_bRecruit;
        bool                m_bBuild;
#endif
    };
}

