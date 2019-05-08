#include "stdafx.h"
#include "GfxHlp.h"
#include "Dlg_Settings.h"
#include "helpers.h"

//#include "xxc/xxc.security.h"

//////////////////////////////////////////////////////////////////////////
iFramePopupView::iFramePopupView(iViewMgr* pViewMgr, PLAYER_ID pid)
: iPopupView(pViewMgr), m_pid(pid)
{}

void iFramePopupView::OnCompose()
{
	ComposeDlgBkgnd(gApp.Surface(), m_Rect);
	iRect rc(m_Rect);
	rc.DeflateRect(12);
	DoCompose(rc);
}

iSize iFramePopupView::PopupViewSize()
{
	iSize result = ClientSize();
	result.w += 24;
	result.h += 24;
	return result;
}

iSize iFramePopupView::PopupViewMinSize()
{
	return iSize(80,45);
}

/*
 *	Common controls
 */
iPHLabel::iPHLabel(iViewMgr* pViewMgr, const iRect& rect, const iStringT& text, Alignment align, iTextComposer::FontConfig fc, TextResId rid)
: iBaseCtrl(pViewMgr, NULL, rect, GENERIC_VIEWPORT, 0, Visible), m_text(text), m_align(align), m_fc(fc), m_rid(rid) {}

void iPHLabel::OnCompose()
{
	iRect rc(GetScrRect());
	iTextComposer::FontConfig fc = m_fc;
	if(m_rid != (uint16)(-1))
		UpdateText();
	gTextComposer.TextOut(fc, gApp.Surface(), rc.point(), m_text, rc, m_align);
}

//////////////////////////////////////////////////////////////////////////
iPHTextBox::iPHTextBox(iViewMgr* pViewMgr, const iRect& rect, const iStringT& text, iTextComposer::FontConfig fc)
: iBaseCtrl(pViewMgr, NULL, rect, GENERIC_VIEWPORT, 0, Visible), m_text(text), m_fc(fc) {}

void iPHTextBox::OnCompose()
{
	iRect rc(GetScrRect());
	iTextComposer::FontConfig fc = m_fc;
	gTextComposer.TextBoxOut(fc, gApp.Surface(), m_text, rc);
}

//////////////////////////////////////////////////////////////////////////
iTBSplitter::iTBSplitter(iViewMgr* pViewMgr, const iRect& rect)
: iBaseCtrl(pViewMgr, NULL, rect, GENERIC_VIEWPORT, 0, Visible)
{}

void iTBSplitter::OnCompose()
{
	iRect rc(GetScrRect());
	gGfxMgr.BlitTile(PDGG(BKTILE), gApp.Surface(), rc);
	ButtonFrame(gApp.Surface(),rc,0);
	gApp.Surface().Darken25Rect(rc);
}
//////////////////////////////////////////////////////////////////////////
//
//iTextButton::iTextButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, TextResId textKey, uint32 uid, uint32 state)
//: iButton(pViewMgr, pCmdHandler, rect, uid, state), m_TextKey(textKey)
//{}
//
//void iTextButton::OnBtnDown() const
//{
////	gSfxMgr.PlaySound(CSND_BUTTON);
//}
//
//void iTextButton::OnCompose()
//{
//	ComposeDlgTextButton(gApp.Surface(),GetScrRect(),GetButtonState(),m_TextKey);
//}

//////////////////////////////////////////////////////////////////////////
iTransparentButton::iTransparentButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, TextResId textKey, uint32 uid, uint32 state)
: iButton(pViewMgr, pCmdHandler, rect, uid, state), m_TextKey(textKey)
{}

void iTransparentButton::OnBtnDown() const
{
	//	gSfxMgr.PlaySound(CSND_BUTTON);
}

void iTransparentButton::OnCompose()
{
	// Compose text
	if(m_TextKey != TRID_VOID) {
//		iTextComposer::FontConfig fc (GetButtonFont(state));
		gTextComposer.TextOut(btnfc_normal,gApp.Surface(),GetScrRect(),gTextMgr[m_TextKey],GetScrRect(), AlignCenter, (m_state & iButton::Pressed)?iPoint(1,1):iPoint());
	}
}

//////////////////////////////////////////////////////////////////////////
iIconButton::iIconButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, SpriteId sid, uint32 uid, uint32 state)
: iButton(pViewMgr, pCmdHandler, rect, uid, state), m_spriteId(sid)
{}

void iIconButton::OnBtnDown() const
{
//	gSfxMgr.PlaySound(CSND_BUTTON);
}

void iIconButton::OnCompose()
{
	ComposeIconButton(gApp.Surface(),GetScrRect(),GetButtonState(),m_spriteId);
}

//////////////////////////////////////////////////////////////////////////
iDlgIconButton::iDlgIconButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, SpriteId sid, uint32 uid, uint32 state)
: iButton(pViewMgr, pCmdHandler, rect, uid, state), m_spriteId(sid)
{}

void iDlgIconButton::OnBtnDown() const
{
//	gSfxMgr.PlaySound(CSND_BUTTON);
}

void iDlgIconButton::OnCompose()
{
	ComposeDlgIconButton(gApp.Surface(),GetScrRect(),GetButtonState(),m_spriteId);
}

//////////////////////////////////////////////////////////////////////////
iCheckButton::iCheckButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iPoint pt, SpriteId sid, SpriteId psid, uint32 uid, uint32 state, bool bChecked)
: iImgBtn(pViewMgr, this, iRect(pt, sm_defSize), uid, sid, psid, iStringT(), state), m_pOwnCmdHandler(pCmdHandler), m_spriteId(sid), m_bChecked(bChecked)
{}

void iCheckButton::SetChecked(bool bChecked, bool bNotify) 
{ 
	m_bChecked = bChecked; 
	SetHighlighted(bChecked);
	if (bNotify) m_pOwnCmdHandler->iCMDH_ControlCommand(this, CCI_CHECKED, m_bChecked);
}

void iCheckButton::OnCompose()
{
	iImgBtn::OnCompose();
}

void iCheckButton::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{//
	SetChecked(!m_bChecked);
	m_pOwnCmdHandler->iCMDH_ControlCommand(pView, cmd, param);
}

iSize iCheckButton::sm_defSize = iSize(30, 77);


//////////////////////////////////////////////////////////////////////////
iIphoneIconButton::iIphoneIconButton(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, SpriteId sid, uint32 uid, uint32 state)
: iButton(pViewMgr, this, rect, uid, state), m_pOwnCmdHandler(pCmdHandler), m_spriteId(sid)
{}

void iIphoneIconButton::OnCompose()
{
	uint32 state = GetButtonState();
	iRect rc = GetScrRect();
	ComposeIconButton(gApp.Surface(),rc,state,m_spriteId);
	rc.InflateRect(1);
	gApp.Surface().FrameRect(rc,cColor_Black);
}

void iIphoneIconButton::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	m_pOwnCmdHandler->iCMDH_ControlCommand(pView, cmd, param);
}

//////////////////////////////////////////////////////////////////////////
iCheckBox::iCheckBox(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, const iStringT& text, uint32 uid, uint32 state, bool bChecked)
: iButton(pViewMgr, this, rect, uid, state), m_pOwnCmdHandler(pCmdHandler), m_text(text), m_bChecked(bChecked)
{}

void iCheckBox::SetChecked(bool bChecked, bool bNotify) 
{ 
	m_bChecked = bChecked; 
	if (bNotify) m_pOwnCmdHandler->iCMDH_ControlCommand(this, CCI_CHECKED, m_bChecked);
}

void iCheckBox::OnCompose()
{
	uint32 state = GetButtonState();
	if (m_bChecked) state |= Selected;
	iRect rc = GetScrRect();

// Cotulla: change for new main menu settings, but other dialogs also changed (?)
#if 0
	iRect icnRect(rc.x, rc.y+1, 11,11);
	DrawRoundRect(gApp.Surface(), icnRect, cColor_Black, RGB16(63,32,0));
	icnRect.DeflateRect(1);
	/*
	gApp.Surface().FrameRect(icnRect, cColor_Black);
	icnRect.DeflateRect(1);
	gApp.Surface().Darken50Rect(icnRect);
	*/
	
	if (m_bChecked) BlitIcon(gApp.Surface(), PDGG(CHECKBOX), icnRect);
	if (state & Pressed) gApp.Surface().Darken25Rect(icnRect);
	iTextComposer::FontConfig fc(GetButtonFont(state));
	gTextComposer.TextOut(fc,gApp.Surface(),iPoint(rc.x+14, rc.y), m_text, iRect(rc.x+14, rc.y, rc.w-14, rc.h),AlignTopLeft);

#else
	iRect icnRect(rc.x, rc.y+1, 16, 16);
	if(m_bChecked) gGfxMgr.Blit(PDGG(CHECKBOX_CHECKED), gApp.Surface(), icnRect);
	else gGfxMgr.Blit(PDGG(CHECKBOX_UNCHECKED), gApp.Surface(), icnRect);
	iTextComposer::FontConfig fc(GetButtonFont(state));
	gTextComposer.TextOut(fc,gApp.Surface(),iPoint(rc.x+19, rc.y), m_text, iRect(rc.x+19, rc.y, rc.w-19, rc.h),AlignTopLeft);

#endif
}

void iCheckBox::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	SetChecked(!m_bChecked);
	m_pOwnCmdHandler->iCMDH_ControlCommand(pView, cmd, m_bChecked);
}


//////////////////////////////////////////////////////////////////////////
// iNMCheckBox - check box used in MainMenu -Cotulla
// if game will have same control, it should be merged with iCheckBox
//////////////////////////////////////////////////////////////////////////

iNMCheckBox::iNMCheckBox(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, const iStringT& text, uint32 uid, uint32 state, bool bChecked)
: iButton(pViewMgr, this, rect, uid, state), m_pOwnCmdHandler(pCmdHandler), m_text(text), m_bChecked(bChecked)
{}

void iNMCheckBox::SetChecked(bool bChecked, bool bNotify) 
{ 
    m_bChecked = bChecked; 
    if (bNotify) m_pOwnCmdHandler->iCMDH_ControlCommand(this, CCI_CHECKED, m_bChecked);
}

void iNMCheckBox::OnCompose()
{
    uint32 state = GetButtonState();
    if (m_bChecked) state |= Selected;
    iRect rc = GetScrRect();
	//gApp.Surface().FrameRect(rc, cColor_Red);
    iSize sz = gGfxMgr.Dimension(PDGG(NMENU_CBOX_P));

    //iRect icnRect(rc.x, rc.y+1, 16, 16);
    iRect icnRect(rc.x + (rc.w - sz.w) / 2, rc.y + (rc.h - sz.h) / 2, sz.w, sz.h);
    if (m_bChecked) 
        gGfxMgr.Blit(PDGG(NMENU_CBOX_P), gApp.Surface(), icnRect);
    else 
        gGfxMgr.Blit(PDGG(NMENU_CBOX_N), gApp.Surface(), icnRect);
    iTextComposer::FontConfig fc(GetButtonFont(state));
    gTextComposer.TextOut(fc,gApp.Surface(),iPoint(rc.x+19, rc.y), m_text, iRect(rc.x+19, rc.y, rc.w-19, rc.h),AlignTopLeft);
}

void iNMCheckBox::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
    SetChecked(!m_bChecked);
    m_pOwnCmdHandler->iCMDH_ControlCommand(pView, cmd, m_bChecked);
}
//////////////////////////////////////////////////////////////////////////
iPHScrollBar::iPHScrollBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, uint32 flags)
: iScrollBar(pViewMgr, pCmdHandler, rect, uid, flags)
{}

void iPHScrollBar::ComposeSBElement(Element el, const iRect& rc, uint32 flags)
{ 
	if (el == EL_Bkgnd) {
		gGfxMgr.BlitTile(PDGG(BKTILE), gApp.Surface(), rc);
		gApp.Surface().Darken50Rect(rc);
		gApp.Surface().FrameRect(iRect(rc.x-1,rc.y-1,rc.w+2,rc.h+2),cColor_Black);
	} else if (el == EL_Thumb) {
		SpriteId bksid = (!IsEnabled())?PDGG(BKTILE):PDGG(CTILE);
		gGfxMgr.BlitTile(bksid, gApp.Surface(), rc);
		ComposeDlgButton(gApp.Surface(), rc, (flags&iButton::Disabled)?iButton::Disabled:0);
	} else {
		iRect orc(rc);
		if (el == EL_BtnUp) {
			orc.h -= 1;
			ComposeDlgIconButton(gApp.Surface(), orc, flags, PDGG(SCRBAR_BTNS)+0);
		} else if (el == EL_BtnDown) {
			orc.y += 1;
			orc.h -= 1;
			ComposeDlgIconButton(gApp.Surface(), orc, flags, PDGG(SCRBAR_BTNS)+1);
		} else if (el == EL_BtnLeft) {
			orc.w -= 1;
			ComposeDlgIconButton(gApp.Surface(), orc, flags, PDGG(SCRBAR_BTNS)+2);
		} else if (el == EL_BtnRight) {
			orc.x += 1;
			orc.w -= 1;
			ComposeDlgIconButton(gApp.Surface(), orc, flags, PDGG(SCRBAR_BTNS)+3);
		} else {
			check(0);
		}
	}
}

// Mini slider (used as slider)
iPHMiniSlider::iPHMiniSlider(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
: iScrollBar(pViewMgr, pCmdHandler, rect, uid, Horizontal)
{}

void iPHMiniSlider::ComposeSBElement(Element el, const iRect& rc, uint32 flags)
{ 
	if (el == EL_Bkgnd) {
		gGfxMgr.BlitTile(PDGG(BKTILE), gApp.Surface(), rc);
		gApp.Surface().Darken50Rect(rc);
		gApp.Surface().FrameRect(iRect(rc.x-1,rc.y-1,rc.w+2,rc.h+2),cColor_Black);
	} else if (el == EL_Thumb) {
		SpriteId bksid = (!IsEnabled())?PDGG(BKTILE):PDGG(CTILE);
		gGfxMgr.BlitTile(bksid, gApp.Surface(), rc);
		ComposeDlgButton(gApp.Surface(), rc, 0);
		//gApp.Surface().FrameRect(iRect(rc.x-1,rc.y-1,rc.w+2,rc.h+2),cColor_Black);
	} else {
		iRect orc(rc);
		if (el == EL_BtnLeft) {
			orc.w -= 1;
			ComposeDlgIconButton(gApp.Surface(), orc, flags, PDGG(MINISLD_BTNS)+1);
		} else if (el == EL_BtnRight) {
			orc.x += 1;
			orc.w -= 1;
			ComposeDlgIconButton(gApp.Surface(), orc, flags, PDGG(MINISLD_BTNS)+0);
		} else {
			check(0);
		}
	}
}

iSize iIphoneBtn::sm_defSize = iSize(65, 50);
iSize iIphoneBtn::sm_defSizeDeluxe = iSize(82, 69);


iSize iIphoneBtn::sm_defPerkSize = iSize(50, 55);

void iIphoneBtn::OnCompose()
{
	//gApp.Surface().FrameRect(GetScrRect(), cColor_White);
	iRect rc = AlignRect(gGfxMgr.Dimension(PDGG(BTN_FRAME)), GetScrRect(), m_align);	
	if(m_img == -1) return;
	//iSize dim = gGfxMgr.Dimension(m_img);
	//iRect dst = AlignRect(dim, rc, AlignCenter);

	

	if(m_bUnclickable || (!(m_state & Pressed) && !m_high))
		gGfxMgr.Blit(PDGG(BTN_FRAME), gApp.Surface(), AlignRect(gGfxMgr.Dimension(PDGG(BTN_FRAME)), rc, AlignCenter));

	gGfxMgr.Blit(m_img, gApp.Surface(), AlignRect(gGfxMgr.Dimension(m_img), rc, AlignCenter));			

	if(!m_bUnclickable && ((m_state & Pressed) || m_high)) {
		gGfxMgr.Blit(PDGG(BTN_FRAME_PRESSED), gApp.Surface(), AlignRect(gGfxMgr.Dimension(PDGG(BTN_FRAME_PRESSED)), rc, AlignCenter));
	}
}

void iImgBtn::OnCompose() {
	if(m_sid == -1)
		return;
	iRect rect = GetScrRect();
	//gApp.Surface().FrameRect(rect, cColor_White);
	iSize dim = gGfxMgr.Dimension(m_sid);
	iRect dst = AlignRect(dim, rect, m_align);
	
#if defined( PC_VERSION )
	if( m_bHighBtn )
		m_high = true;
#endif

    sint16 pressed_sid = 
#ifdef FULL_VERSION
    PDGG(BTN_DELUXE_PRESSED);
#else
    PDGG(BTN_FRAME_PRESSED);
#endif
    
	if((!m_high && !(m_state & Pressed)) || m_bUnclickable) {
		if(m_sid != -1) {
			gGfxMgr.Blit(m_sid, gApp.Surface(), dst);
		}
	} else {
		if(m_pressedSid != -1) {
			gGfxMgr.Blit(m_pressedSid, gApp.Surface(), dst);
		} else if(m_sid != -1) {
			gGfxMgr.Blit(m_sid, gApp.Surface(), dst);
			if(GetScrRect().size() == iIphoneBtn::sm_defSize || GetScrRect().size() == iIphoneBtn::sm_defSizeDeluxe) {
				gGfxMgr.Blit(pressed_sid, gApp.Surface(), AlignRect(gGfxMgr.Dimension(pressed_sid), dst, AlignCenter));
            }
		}
	} 
#if defined( PC_VERSION )
	if( m_bEntered && text.Length() ){
		
			iSize sz = gTextComposer.GetTextSize(text, btnfc_normal);
			iSize szcntx_l = gGfxMgr.Dimension(PDGG(CONTEXT_L));

			iRect cntx = iRect();
			iPoint txtpnt = iPoint();
			iPoint pos = iPoint();
			
			if( m_align == AlignTopRight || m_align == AlignRight ){
	
				pos = iPoint( rect.x2(), rect.y1() + 2*rect.size().h/3 );
			
				cntx = iRect( pos.x - 2*szcntx_l.w - sz.w, pos.y, szcntx_l.w, szcntx_l.h );
				gGfxMgr.Blit(PDGG(CONTEXT_L), gApp.Surface(), cntx);
			
				cntx = iRect( pos.x - szcntx_l.w - sz.w, pos.y, sz.w, szcntx_l.h );		
				gGfxMgr.BlitTile(PDGG(CONTEXT_M), gApp.Surface(), cntx);
			
				cntx = iRect( pos.x - szcntx_l.w, pos.y, szcntx_l.w, szcntx_l.h );		
				gGfxMgr.Blit(PDGG(CONTEXT_R), gApp.Surface(), cntx);
			
				txtpnt = iPoint(pos.x - szcntx_l.w - sz.w, pos.y+(szcntx_l.h - sz.h)/2);
			}
			else if( m_align == AlignTopLeft ){
		
				pos = iPoint( rect.x1(), rect.size().h/2);
			
				gGfxMgr.Blit(PDGG(CONTEXT_L), gApp.Surface(), pos);
		
				cntx = iRect( pos.x+szcntx_l.w, pos.y, sz.w, szcntx_l.h );		
				gGfxMgr.BlitTile(PDGG(CONTEXT_M), gApp.Surface(), cntx);
		
				gGfxMgr.Blit(PDGG(CONTEXT_R), gApp.Surface(), iPoint( pos.x+szcntx_l.w+sz.w, pos.y ) );
		
				txtpnt = iPoint(pos.x+szcntx_l.w, pos.y+(szcntx_l.h - sz.h)/2);
			}		
			else if(m_align == AlignBottomLeft || m_align == AlignBottomRight){
							
				iSize limsize = szcntx_l + iSize(5, 0);
                iStringT limtext = text;
/*                
                if( sz.w > limsize.w ){
        
                    iSize stext;
                    
                    limtext.SetLength(text.Length()-2);
                    limtext += _T("..");
                    
                    stext = gTextComposer.GetTextSize(limtext, btnfc_normal);
                    
                    while( stext.w > limsize.w ){
                    
                        limtext.SetLength(limtext.Length()-3);
                        limtext += _T("..");
                        stext = gTextComposer.GetTextSize(limtext, btnfc_normal);
                    }
                }
*/                
                sz = gTextComposer.TextCut(limtext, limsize.w, btnfc_normal);//  gTextComposer.GetTextSize(limtext, btnfc_normal);
                
                pos = iPoint( rect.x2() + szcntx_l.w + sz.w/2 - rect.size().w/2, rect.y1() - 2*rect.size().h/3 );
                
				cntx = iRect( pos.x - 2*szcntx_l.w - sz.w, pos.y, szcntx_l.w, szcntx_l.h );
				gGfxMgr.Blit(PDGG(CONTEXT_L), gApp.Surface(), cntx);
                
				cntx = iRect( pos.x - szcntx_l.w - sz.w, pos.y, sz.w, szcntx_l.h );		
				gGfxMgr.BlitTile(PDGG(CONTEXT_M), gApp.Surface(), cntx);
                
				cntx = iRect( pos.x - szcntx_l.w, pos.y, szcntx_l.w, szcntx_l.h );		
				gGfxMgr.Blit(PDGG(CONTEXT_R), gApp.Surface(), cntx);
                
				txtpnt = iPoint(pos.x - szcntx_l.w - sz.w, pos.y+(szcntx_l.h - sz.h)/2);
                
                gTextComposer.TextOut(btnfc_normal, gApp.Display().GetSurface(), txtpnt, limtext, cInvalidRect, AlignLeft);
                return;
			}
            else if(m_align == AlignBottom ){
            
                pos = iPoint( rect.x2() + szcntx_l.w + sz.w/2 - rect.size().w/2, rect.y1() - 2*rect.size().h/3 );
                
				cntx = iRect( pos.x - 2*szcntx_l.w - sz.w, pos.y, szcntx_l.w, szcntx_l.h );
				gGfxMgr.Blit(PDGG(CONTEXT_L), gApp.Surface(), cntx);
                
				cntx = iRect( pos.x - szcntx_l.w - sz.w, pos.y, sz.w, szcntx_l.h );		
				gGfxMgr.BlitTile(PDGG(CONTEXT_M), gApp.Surface(), cntx);
                
				cntx = iRect( pos.x - szcntx_l.w, pos.y, szcntx_l.w, szcntx_l.h );		
				gGfxMgr.Blit(PDGG(CONTEXT_R), gApp.Surface(), cntx);
                
				txtpnt = iPoint(pos.x - szcntx_l.w - sz.w, pos.y+(szcntx_l.h - sz.h)/2);
            }
      
			gTextComposer.TextOut(btnfc_normal, gApp.Display().GetSurface(), txtpnt, text, cInvalidRect, AlignLeft);
	}
#endif
}

// amount chooser
iAmountChooser::iAmountChooser(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid):
iBaseCtrl(pViewMgr, pCmdHandler, rect, GENERIC_VIEWPORT, uid, Visible | Enabled), m_pos(0), m_max(0)
{
//	AddChild(new iImgBtn(&gApp.ViewMgr(), this, iRect(0, 0, 58, 45), 101, PDGG(BTN_RECRUIT_MAX), Visible | Enabled));
}

void iAmountChooser::OnMouseTrack(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	iRect rc = GetScrRect();
	if(!rc.PtInRect(pos.x, pos.y)) return;
	m_pos = iCLAMP<sint32>(0, m_max, m_max * (rc.y + (rc.h) - pos.y) / (rc.h - 20));
	Invalidate();
	if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_BTNCLICK,m_pos);	
}

void iAmountChooser::OnMouseDown(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	OnMouseTrack(pos, mID, mbID);
}

void iAmountChooser::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	m_pos = m_max;
	if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_BTNCLICK,m_pos);	
	Invalidate();
}


void iAmountChooser::OnCompose()
{
	iRect rc = GetScrRect();
	rc.y += 20;
	rc.h -= 20;
	gGfxMgr.Blit(PDGG(AMOUNT_CONTROL), gApp.Surface(), rc);
	rc.DeflateRect(0, 5, 0, 0);
//	rc.y += 3;
	if(m_max > 0) {
		uint32 barCnt = m_pos * (rc.h) / m_max / 3;
		for(uint32 x=0; x<barCnt; x++)
			gGfxMgr.Blit(PDGG(AMOUNT_CONTROL_BAR), gApp.Surface(), iPoint(rc.x + 4, rc.y + rc.h - (x*3) - 5));
	}
}


//
// horizontal amount chooser with glow interface
const iSize iAmountChooserGlow::sm_defSize = iSize(399 + 13*2, 63);//47);
#ifdef PC_VERSION
const sint32 iAmountChooserGlow::sm_offset = 137;
#else
const sint32 iAmountChooserGlow::sm_offset = 257;
#endif

iAmountChooserGlow::iAmountChooserGlow(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, bool bDeluxe):
iBaseCtrl(pViewMgr, pCmdHandler, iRect(rect.x, rect.y, rect.w 
#ifdef PC_VERSION
                                       + 25
#endif                      
                                       , rect.h), GENERIC_VIEWPORT, uid, Visible | Enabled), m_pos(0), m_max(0), m_bDeluxe(bDeluxe)
{	
    sint16 sid = bDeluxe ? PDGG(BTN_SWITCH_CASTLES_DELUXE): PDGG(BTN_SWITCH_CASTLES);

	AddChild(m_pBtnLeft = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 114, sid, sid + 2));
	AddChild(m_pBtnRight = new iImgBtn(&gApp.ViewMgr(), this, iRect(), 115, sid + 1, sid + 3));		
    
    UpdateSize();
}

void iAmountChooserGlow::UpdateSize()
{

    if( !m_bDeluxe ){
#ifdef PC_VERSION
        m_pBtnLeft->SetRect(AlignChild(iSize(50, 50), AlignLeft) + iPoint(10, 0));
        m_pBtnRight->SetRect(AlignChild(iSize(50, 50), AlignRight));                    
#endif
    }
    else{
        m_pBtnLeft->SetRect(AlignChild(iSize(63, 63), AlignLeft));
        m_pBtnRight->SetRect(AlignChild(iSize(63, 63), AlignRight));
    }
}

double posit=0.0;

void iAmountChooserGlow::OnMouseTrack(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	iRect rc = AlignRect(iSize(
#ifdef PC_VERSION
                               sm_defSize.w - 
#endif
                               sm_offset, sm_defSize.h), GetScrRect(), AlignCenter);

	if(!rc.PtInRect(pos.x, pos.y)) return;
#ifdef PC_VERSION
    uint32 lw_offset = 0, rw_offset = 0;

    if( m_bDeluxe ){
    
        lw_offset = 15;
        rw_offset = 20;
    }    
    
	sint32 lw = gGfxMgr.Dimension(m_bDeluxe ? PDGG(AMOUNT_CONTROL_DELUXE_L): PDGG(AMOUNT_CONTROL_GLOW_L)).w-lw_offset,
    rw = gGfxMgr.Dimension(m_bDeluxe ? PDGG(AMOUNT_CONTROL_DELUXE_R): PDGG(AMOUNT_CONTROL_GLOW_R)).w-rw_offset;
#else
    sint32 lw = gGfxMgr.ObjRect(m_bDeluxe ? PDGG(AMOUNT_CONTROL_DELUXE_L) : PDGG(AMOUNT_CONTROL_GLOW_L)).w,
    rw = gGfxMgr.ObjRect(m_bDeluxe ? PDGG(AMOUNT_CONTROL_DELUXE_R) : PDGG(AMOUNT_CONTROL_GLOW_R)).w;
#endif
	if(pos.x <= rc.x + lw) 
		m_pos = 0;
	else if(pos.x >= rc.x 
#ifdef PC_VERSION
            + sm_defSize.w - sm_offset
#else
            + sm_offset
#endif
            - rw)
		m_pos = m_max;
	else{
#ifdef PC_VERSION        
		m_pos = iCLAMP<sint32>(0, m_max, m_max * (pos.x - rc.x - lw) / (sm_defSize.w - sm_offset - rw - lw)+1);
#else
        m_pos = iCLAMP<sint32>(0, m_max, m_max * (pos.x - rc.x) / (sm_offset));
#endif        
//        posit = (double)(m_max * (pos.x - rc.x - lw)) / (double)(sm_defSize.w - sm_offset - rw - lw);
    }
	Invalidate();
	if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_BTNCLICK,m_pos);	
}

void iAmountChooserGlow::OnMouseDown(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	OnMouseTrack(pos, mID, mbID);
}

void iAmountChooserGlow::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if(uid == 114) {
		m_pos = iCLAMP<sint32>(0, m_max, m_pos - 1);
	} else if(uid == 115) {
		m_pos = iCLAMP<sint32>(0, m_max, m_pos + 1);		
	}
	if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_BTNCLICK,m_pos);	
	Invalidate();
}


void iAmountChooserGlow::OnCompose()
{
	iRect rc = AlignRect(iSize(
#ifdef PC_VERSION
                               sm_defSize.w - 
#endif
                               sm_offset, sm_defSize.h), GetScrRect(), AlignCenter);

#ifndef PC_VERSION
    if(m_bDeluxe)
		rc -= iPoint(15, 0);
#endif
    
    uint32 lw_offset = 0, rw_offset = 0;
#ifdef PC_VERSION
    if( m_bDeluxe ){
        
        lw_offset = 15;
        rw_offset = 20;
    }  
#endif
	sint32 lw = gGfxMgr.Dimension(m_bDeluxe ? PDGG(AMOUNT_CONTROL_DELUXE_L) : PDGG(AMOUNT_CONTROL_GLOW_L)).w-lw_offset,
			rw = gGfxMgr.Dimension(m_bDeluxe ? PDGG(AMOUNT_CONTROL_DELUXE_R) : PDGG(AMOUNT_CONTROL_GLOW_R)).w-rw_offset;
    
    iSize sz = gGfxMgr.Dimension(PDGG(AMOUNT_CONTROL_DELUXE));
    
	gGfxMgr.Blit(m_bDeluxe ? PDGG(AMOUNT_CONTROL_DELUXE) : PDGG(AMOUNT_CONTROL_GLOW), gApp.Surface(), rc);	

	iPoint offset;

	if(!m_bDeluxe)
		offset = iPoint(7, 8);
	else 
		offset = iPoint(-10, -11);
    
    iPoint trc;
	if(m_max > 0) {
#ifdef PC_VERSION
		sint32 barCnt = m_pos* (sm_defSize.w - sm_offset - lw - rw) / m_max;// - rw - lw;
        for(sint32 x=1; x<barCnt+1; x++){
            
            trc = iPoint(rc.x + x + lw, rc.y) + offset;
#else
        sint32 barCnt = m_pos * (sm_offset - lw - (m_bDeluxe ? 0 : rw)) / m_max;
        for(sint32 x=1; x<barCnt+1; x++){
            
            trc = rc.point() + iPoint(x + lw - 1, 0) + offset;
#endif
            gGfxMgr.Blit(m_bDeluxe ? PDGG(AMOUNT_CONTROL_DELUXE_M): PDGG(AMOUNT_CONTROL_GLOW_M), gApp.Surface(), trc);
        }
    }
	if(m_pos > 0) {
        
        trc = rc.point() + offset;
        
		gGfxMgr.Blit(m_bDeluxe ? PDGG(AMOUNT_CONTROL_DELUXE_L) : PDGG(AMOUNT_CONTROL_GLOW_L), gApp.Surface(), trc);
		if(m_pos == m_max){
            
#ifdef PC_VERSION
            uint32 offs = 0;
            if(m_bDeluxe)
                offs = 31;
            else
                offs = rw+4;
            
            trc = rc.point() + iPoint(sm_defSize.w - sm_offset - offs, 0) + offset;
#else
            trc = rc.point() + iPoint(sm_offset - (m_bDeluxe ? 0 : (rw+1)), 0) + offset;
#endif            
			gGfxMgr.Blit(m_bDeluxe ? PDGG(AMOUNT_CONTROL_DELUXE_R) : PDGG(AMOUNT_CONTROL_GLOW_R), gApp.Surface(), trc);
        }
	}
}


//
// 1 to 5 stars rating chooser
const iSize iRater::sm_defSize = iSize(265, 73);

iRater::iRater(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid):
iBaseCtrl(pViewMgr, pCmdHandler, iRect(rect.x, rect.y, rect.w, rect.h), GENERIC_VIEWPORT, uid, Visible | Enabled), m_pos(0), m_alpha(255)
{	
}

void iRater::UpdateSize()
{
}

void iRater::OnMouseTrack(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	iRect rc = GetScrRect(); //AlignRect(iSize(sm_defSize.w, sm_defSize.h), GetScrRect(), AlignCenter);
	
	if(!rc.PtInRect(pos.x, pos.y)) return;
	
	if(pos.x <= rc.x ) 
		m_pos = 0;
	else if(pos.x >= rc.x + sm_defSize.w)
		m_pos = 4;
	else
		m_pos = iCLAMP<sint32>(0, 4, (pos.x - rc.x) / (sm_defSize.w / 5));
	Invalidate();
//	if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_BTNCLICK,m_pos);	
}

void iRater::OnMouseDown(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	OnMouseTrack(pos, mID, mbID);
}

void iRater::OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_BTNCLICK, m_pos);		
}

void iRater::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
}


void iRater::OnCompose()
{
	iRect rc = GetScrRect();
	rc.w = rc.w / 5;
	for(uint32 xx=0; xx<5; xx++)
	{
		sint16 sid;
		if(IsMouseDown(0) && xx <= m_pos)
			sid = PDGG(MAPRATE_STAR_PRESSED);
		else if(xx <= m_pos)
			sid = PDGG(MAPRATE_STAR_HIGHLIGHTED);
		else
			sid = PDGG(MAPRATE_STAR);

		gGfxMgr.BlitAlpha(sid, gApp.Surface(), rc.point(), m_alpha);
		rc.x += rc.w;
	}
}



//
// Twitter pane
// 

const iSize iTwitterPane::sm_defSize = iSize(360, 220);

iTwitterPane::iTwitterPane(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid):
iBaseCtrl(pViewMgr, pCmdHandler, iRect(rect.x, rect.y, rect.w, rect.h), GENERIC_VIEWPORT, uid, Visible | Enabled),
m_pHandler(pCmdHandler)
{	
	AddChild(m_pRater = new iRater(pViewMgr, this, iRect(), 100));
	AddChild(m_pBtn = new iImgBtn(pViewMgr, this, iRect(), 101, PDGG(TWEET), PDGG(TWEET_PRESSED)));
	m_pRater->SetVisible(false);
}

void iTwitterPane::UpdateSize()
{
	m_pRater->SetRect(AlignChild(iRater::sm_defSize, AlignBottomRight));
	iSize sz = gGfxMgr.Dimension(PDGG(TWEET));
	m_pBtn->SetRect(AlignChild(sz, AlignLeft));
	sz.h -= 45;
	m_pBtn->SetSize(sz);
}

void iTwitterPane::OnMouseTrack(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
	
}

void iTwitterPane::OnMouseDown(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
}

void iTwitterPane::OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
}


void iTwitterPane::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	uint32 uid = pView->GetUID();
	if(uid == 101) {
		m_pHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, 0);
		//m_pRater->SetVisible(!m_pRater->IsVisible());
	} else if(uid == 100) {
		m_pHandler->iCMDH_ControlCommand(this, CCI_LBSELECTED, m_pRater->CurPos());		
		m_pHandler->iCMDH_ControlCommand(this, CCI_BTNCLICK, 0);
	}
}


void iTwitterPane::Reset()
{
	m_pRater->SetVisible(false);
}

void iTwitterPane::ShowRating()
{
	m_pRater->SetVisible(true);
	m_pRater->SetCurPos(4);
}

void iTwitterPane::OnCompose()
{
	//gApp.Surface().FrameRect(GetScrRect(), 0);
	gTextComposer.TextOut(grfc_title, gApp.Surface(), iPoint(), _T("Tweet this?"), GetScrRect(), AlignRight);
}



sint32 iTwitterPane::SelectedRating() const
{
	return m_pRater->CurPos();
}

// volume bar
iPHVolumeBar::iPHVolumeBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid):
iBaseCtrl(pViewMgr, pCmdHandler, rect, GENERIC_VIEWPORT, uid, Visible | Enabled), m_pos(0)
{
	
}

void iPHVolumeBar::OnCompose()
{
	iRect rc = GetScrRect();
	gGfxMgr.Blit(PDGG(SETTINGS_BAR), gApp.Surface(), rc);
	rc.y += 3;
	rc.x += 2;
	for(uint32 x=0; x<m_pos + 1; x++) {
		gGfxMgr.Blit(PDGG(SETTINGS_BAR_CHECK), gApp.Surface(), rc);
		rc.x += 29;
	}
}

void iPHVolumeBar::OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	iRect rc = GetScrRect();
	m_pos = iCLAMP<sint32>(0, 4, (pos.x - rc.x) / 30);
	if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_BTNCLICK,m_pos);
	Invalidate();
}


void iPHVolumeBar::OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	iRect rc = GetScrRect();
	m_pos = iCLAMP<sint32>(0, 4, (pos.x - rc.x) / 30);
	if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_BTNCLICK,m_pos);
	Invalidate();	
}


void iPHVolumeBar::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
}

//////////////////////////////////////////////////////////////////////////
// volume bar for new main menu -Cotulla
//////////////////////////////////////////////////////////////////////////

iNMVolumeBar::iNMVolumeBar(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid):
iBaseCtrl(pViewMgr, pCmdHandler, rect, GENERIC_VIEWPORT, uid, Visible | Enabled), m_pos(0)
{
}

void iNMVolumeBar::OnCompose()
{
    iRect rc = GetScrRect();
    //gGfxMgr.Blit(PDGG(SETTINGS_BAR), gApp.Surface(), rc);
    rc.y += 3;
    rc.x += 2 + 25;
    for (uint32 x = 0; x < 5; x++)
    {
        gGfxMgr.Blit(x < m_pos ? PDGG(NMENU_SLIDER_P) : PDGG(NMENU_SLIDER_N), gApp.Surface(), rc);
        rc.x += 29;
    }
}

void iNMVolumeBar::OnMouseUp(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
    iRect rc = GetScrRect();
    m_pos = iCLAMP<sint32>(0, /*4*/5, (pos.x - rc.x) / 30);
    if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_BTNCLICK,m_pos);
    Invalidate();
}


void iNMVolumeBar::OnMouseTrack(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
    iRect rc = GetScrRect();
    m_pos = iCLAMP<sint32>(0, /*4*/5, (pos.x - rc.x) / 30);
    if (m_pCmdHandler) m_pCmdHandler->iCMDH_ControlCommand(this,CCI_BTNCLICK,m_pos);
    Invalidate();	
}

void iNMVolumeBar::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
}



/*
 *	Specialized controls
 */

// Tool bar tabbed switch
iBarTabSwitch::iBarTabSwitch(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 tabcnt, uint32 uid)
: iTabbedSwitch(pViewMgr, pCmdHandler, rect, tabcnt, uid) 
{}

void iBarTabSwitch::SetTabIcon(SpriteId sid)
{
	m_Tabs.Add(sid);
}

void iBarTabSwitch::ComposeTabItem(uint32 idx, uint32 itemState, const iRect& rect)
{
	ComposeIconButton(gApp.Surface(),rect,itemState,m_Tabs[idx]);
	if (!IsEnabled()) gApp.Surface().Darken25Rect(rect);
}


iHeroPortBtn::iHeroPortBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect rect, uint32 uid, bool bUnclickable, bool bShowMoves, Alignment align
#ifndef PC_VERSION
                           , bool bGUI
#endif
                           )
: iIphoneBtn(pViewMgr, pCmdHandler, rect, uid, Visible, PDGG(UNK_HERO), false, align), m_pHero(NULL), m_bUnclickable(bUnclickable), m_bShowMoves(bShowMoves) 
#ifndef PC_VERSION
, m_bGUI(bGUI)
#endif
{}

void iHeroPortBtn::OnCompose()
{
	if(m_pHero){
#ifndef PC_VERSION	
        if(m_bGUI)
#endif
			iIphoneBtn::OnCompose();	
#ifndef PC_VERSION
		else {
			iRect rc = AlignRect(gGfxMgr.Dimension(PDGG(BTN_FRAME)), GetScrRect(), m_align);	
			
			gGfxMgr.Blit(m_img, gApp.Surface(), AlignRect(gGfxMgr.Dimension(m_img), rc, AlignCenter));			
		}
#endif
    }
	if(m_bShowMoves && m_pHero) {	
		iRect rect = AlignRect(gGfxMgr.Dimension(PDGG(BTN_FRAME)), GetScrRect(), m_align);
		iSize dim = gGfxMgr.Dimension(m_img);
		iRect rc  = AlignRect(dim, rect, AlignCenter);		

#ifndef ROYAL_BOUNTY
		iRect movebar = AlignRect(iSize(46, 3), rc, AlignBottom);// + iPoint(1, -1);
		iRect tofill = AlignRect(iSize(iCLAMP<sint32>(0, movebar.w, m_pHero->Moves() * movebar.w / m_pHero->TotalMoves()), movebar.h), movebar, AlignLeft);
		gApp.Surface().FillRect(tofill, cColor_Green);
		if(m_pHero->Exhausted())
			gApp.Surface().DarkenBWRect(rc);		
#else
        if(m_pHero->MaxManaPts() > 0) {
			iRect manabar = AlignRect(iSize(46, 3), rc, AlignBottom);// + iPoint(1, -1);
			iRect tofill = AlignRect(iSize(iCLAMP<sint32>(0, manabar.w, m_pHero->ManaPts() * manabar.w / m_pHero->MaxManaPts()), manabar.h), manabar, AlignLeft);
			gApp.Surface().FillRect(tofill, cColor_Blue);
		}
#endif
    }
}

void iHeroPortBtn::SetHero(iHero* pHero)
{
	m_pHero = pHero;
	SetEnabled(m_pHero != NULL && !m_bUnclickable);
	SetVisible(m_pHero != NULL);
	if(m_pHero) 
		SetSprite(m_pHero->Proto()->m_icn46);
	Invalidate();
}


iBigHeroPortBtn::iBigHeroPortBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
: iButton(pViewMgr, pCmdHandler, rect, uid, Visible|Enabled), m_pHero(NULL), m_bHighlighted(false) {}

void iBigHeroPortBtn::OnCompose()
{
	iRect rc = GetScrRect();
	iRect avrc = AlignRect(iSize(50, 50), rc, AlignTop);
	ComposeHeroAvatar(gApp.Surface(), avrc, m_pHero, (m_state & Pressed) || m_bHighlighted, true);
	if(m_pHero->Exhausted())
		gApp.Surface().DarkenBWRect(avrc);	
	
	// print name		
	rc.y += 55;
	rc.InflateRect(50, 0);		
	rc.h = gTextComposer.GetFontHeight(castleheronamefc.fontSize);
	gTextComposer.TextBoxOut(castleheronamefc, gApp.Surface(), m_pHero->Name(), rc);

	
}

void iBigHeroPortBtn::SetHero(iHero* pHero)
{
	m_pHero = pHero;	
	SetVisible(m_pHero != NULL);			
	Invalidate();
}


iCastleGlyphBtn::iCastleGlyphBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid, bool bUnclick
#ifndef PC_VERSION
                                 , bool bGUI
#endif
                                 )
: iImgBtn(pViewMgr, pCmdHandler, rect, uid, -1, 0-1, iStringT(), Visible|Enabled, AlignCenter, bUnclick) 
#ifndef PC_VERSION
, m_bGUI(bGUI)
#endif
{}


void iCastleGlyphBtn::SetCastle(iCastle* pCastle)
{
	m_pCastle = pCastle;
	
    sint16 sid = 
#ifndef PC_VERSION
    !m_bGUI ? PDGG(BTN_CASTLES_DELUXE): 
#endif
    PDGG(BTN_CASTLES);
    
	if(m_pCastle) {
		SetSprite(sid + m_pCastle->Proto()->Type(), -1);
	} else {
		SetSprite(-1, -1);
	}
	SetEnabled(m_pCastle != NULL);
	SetVisible(m_pCastle != NULL);
	Invalidate();
}


iBigCastleGlyphBtn::iBigCastleGlyphBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
: iButton(pViewMgr, pCmdHandler, rect, uid, Visible|Enabled), m_bHighlighted(false) {}


void iBigCastleGlyphBtn::SetCastle(iCastle* pCastle)
{
	m_pCastle = pCastle;	
	SetVisible(m_pCastle != NULL);
	Invalidate();
}

void iBigCastleGlyphBtn::OnCompose()
{
	iRect rc = GetScrRect();
	
	if(m_pCastle) {
		sint16 sid = PDGG(BTN_CASTLES_BIG) + m_pCastle->Proto()->Type();
		iRect trc = AlignRect(gGfxMgr.Dimension(sid), rc, AlignTop);
		gGfxMgr.Blit(sid, gApp.Surface(), trc.point());
		if((m_state & Pressed) || m_bHighlighted)
			gGfxMgr.Blit(PDGG(BTN_FRAME_BIG_PRESSED), gApp.Surface(), trc.point()- iPoint(4, 4));

		if(!m_pCastle->CanBuild())
			gApp.Surface().DarkenBWRect(trc);

		// print name		
		rc.y += 55;
		rc.InflateRect(50, 0);		
		rc.h = gTextComposer.GetFontHeight(castleheronamefc.fontSize);
        
        iStringT cstlName = m_pCastle->Name(); 
        
        gTextComposer.TextCut(cstlName, 100, castleheronamefc);
        
		gTextComposer.TextBoxOut(castleheronamefc, gApp.Surface(), cstlName, rc);
	}
}

// Spell button
iSpellBtn::iSpellBtn(iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, uint32 uid)
: iButton(pViewMgr, pCmdHandler, rect, uid, Visible), m_pSpell(NULL) {}

void iSpellBtn::OnCompose()
{
	iRect rc = GetScrRect();
	if (m_pSpell){
		BlitIcon(gApp.Surface(), PDGG(SPELLSCROLL_UNROLLED), rc, 255, iPoint(), m_state & Pressed);
		BlitIcon(gApp.Surface(), m_pSpell->Icon(), rc, 255, iPoint(), m_state & Pressed);
		gTextComposer.TextBoxOut(spellscrollfc, gApp.Surface(), gTextMgr[m_pSpell->NameKey()], AlignRect(iSize(rc.w, gTextComposer.GetFontHeight(spellscrollfc.fontSize)), rc, AlignBottom));
	} else {
		BlitIcon(gApp.Surface(), PDGG(SPELLSCROLL_ROLLED), rc);
	}
}

// Popup list box
iPopupList::iPopupList(iViewMgr* pViewMgr, PopupListHandler* pHandler, const iSize& itemMetrics, sint32 itemsCount, sint32 selItem, sint32 pageSize)
: iPopupView(pViewMgr), m_pHandler(pHandler), m_itemMetrics(itemMetrics), m_itemsCount(itemsCount), m_selItem(selItem), m_pageSize(pageSize), m_sidx(-1), m_scrPos(0)
{}

iSize iPopupList::PopupViewSize()
{
	uint32 w = m_itemMetrics.w + 2;
	if (m_itemsCount>m_pageSize) w += 16;
	return iSize(w, m_itemMetrics.h * iMIN<uint32>(m_pageSize,m_itemsCount) + 2);
}

iSize iPopupList::PopupViewMinSize()
{
	return iSize(10,10);
}

void iPopupList::OnTrackPopup(const iRect& clRect)
{
	if (m_itemsCount > m_pageSize) {
		AddChild(m_pScrollBar = new iPHScrollBar(m_pMgr, this, iRect((sint32)(clRect.w)-16,1,15,clRect.h-2), 1024, 0));
		m_pScrollBar->SetMetrics(m_itemsCount, m_pageSize);
		if (m_selItem >= m_pageSize) m_scrPos = (m_selItem-m_pageSize) + 1;
		m_pScrollBar->SetCurPos(m_scrPos);
	}
}

void iPopupList::OnHidePopup()
{
}

void iPopupList::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID) 
{
	sint32 nidx = m_scrPos + (pos.y - m_Rect.y - 3) / (sint32)m_itemMetrics.h;
	if (nidx >= 0 && nidx < m_itemsCount) m_sidx = nidx;
	Invalidate();
}

void iPopupList::OnMouseClick(const iPoint& pos, MouseId mID, MouseButtonId mbID)
{
	sint32 nidx = m_scrPos + (pos.y - m_Rect.y - 3) / (sint32)m_itemMetrics.h;
	if (nidx >= 0 && nidx < m_itemsCount && m_sidx == nidx) {
		sint32 curIdx = m_sidx;
		PopupListHandler* pHandler = m_pHandler;
		m_pMgr->HidePopup();
		(*pHandler)(curIdx);
	} else m_sidx = -1;
}

void iPopupList::OnCompose()
{
	iRect rc(GetScrRect());
	// frame and bkgnd
	gApp.Surface().FrameRect(rc, cColor_Black);
	rc.DeflateRect(1);
	gGfxMgr.BlitTile(PDGG(BKTILE), gApp.Surface(), rc);

	// Items
	iRect brc(rc.x,rc.y,m_itemMetrics.w, m_itemMetrics.h);
	for (sint32 xx=m_scrPos; xx<iMIN<uint32>(m_scrPos+m_pageSize,m_itemsCount); ++xx) {
		DrawItem(xx, gApp.Surface(), brc, ((xx == m_sidx)?iButton::Pressed:0)|((xx == m_selItem)?iButton::Selected:0));
		brc.y += m_itemMetrics.h;
	}
}
void iPopupList::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
	check(pView == m_pScrollBar);
	m_scrPos = param;
	Invalidate();
}

// Hero list popup
static void HeroListHandler(sint32 itemId)
{
	gGame.Map().ActPlayer()->SetCurHero( *(gGame.Map().ActPlayer()->HeroFirst() + itemId) );
}

iHeroListPopup::iHeroListPopup(iViewMgr* pViewMgr)
: iPopupList(pViewMgr, HeroListHandler, iSize(165,26), gGame.Map().ActPlayer()->HeroCount(), gGame.Map().ActPlayer()->CurHeroIdx(), 7)
{
}

void iHeroListPopup::DrawItem(sint32 itemId, iDib& surf, const iRect& rc, uint32 flags)
{
	iPlayer* pPlayer = gGame.Map().ActPlayer();
	iHero* pHero = *(pPlayer->HeroFirst() + itemId);
	ButtonFrame(surf,rc,0 );
	iRect frc(rc);
	if (flags & iButton::Selected) {
		gGfxMgr.BlitTile(PDGG(CTILE), surf,frc);
		ButtonFrame(surf,frc,0);
	}
	frc.DeflateRect(1);

	iTextComposer::FontConfig fc_name(FS_MEDIUM, RGB16(255,255,0));
	// portrait
	surf.FillRect(iRect(frc.x+1,frc.y+1,44,22),cColor_Black);
	SpriteId sid = pHero->Proto()->m_icn32;
	gGfxMgr.Blit(sid,surf, iPoint(frc.x+2,frc.y+2));

	// act points and mana
	ComposeProgressBar(true, gApp.Surface(), iRect(frc.x+35,frc.y+2,4,20), cColor_Green192, pHero->Moves(), 250);
	ComposeProgressBar(true, gApp.Surface(), iRect(frc.x+40,frc.y+2,4,20), RGB16(32,128,255), pHero->ManaPts(), 100);

	// name
	gTextComposer.TextOut(fc_name, gApp.Surface(), iPoint(frc.x+42,frc.y-1), pHero->Name(), iRect(frc.x+42,frc.y,frc.w-42,frc.h),AlignTop);

	// Primary skills
	iStringT psText;
	for (sint32 psid=0; psid<PRSKILL_COUNT; ++psid) {
		sint32 sval = pHero->FurtSkill((FURTHER_SKILLS)(FSK_ATTACK+psid));
		psText.Addf(_T("#I%04X %d "),PDGG(ICN_PSKILLS)+psid, sval);
		if (sval <= 10) psText += _T(" ");
	}
	gTextComposer.TextOut(gApp.Surface(), iPoint(frc.x+42,frc.y+12), psText, iRect(frc.x+42,frc.y+12,frc.w-42,frc.h-12),AlignTop);
}

// Castle list popup
static void CastleListHandler(sint32 itemId)
{
	gGame.Map().ActPlayer()->SetCurCastle( *(gGame.Map().ActPlayer()->CastleFirst() + itemId) );
}

iCastleListPopup::iCastleListPopup(iViewMgr* pViewMgr)
: iPopupList(pViewMgr, CastleListHandler, iSize(151,26), gGame.Map().ActPlayer()->CastleCount(), gGame.Map().ActPlayer()->CurCastleIdx(), 7)
{}

void iCastleListPopup::DrawItem(sint32 itemId, iDib& surf, const iRect& rc, uint32 flags)
{
	iPlayer* pPlayer = gGame.Map().ActPlayer();
	iCastle* pCastle = *(pPlayer->CastleFirst() + itemId);
	ButtonFrame(surf,rc,0 );
	iRect frc(rc);
	if (flags & iButton::Selected) {
		gGfxMgr.BlitTile(PDGG(CTILE), surf,frc);
		ButtonFrame(surf,frc,0);
	}
	frc.DeflateRect(1);

	iTextComposer::FontConfig fc_name(FS_MEDIUM, RGB16(255,255,0));
	// glyph
	surf.FillRect(iRect(frc.x+1,frc.y+1,34,22),cColor_Black);
	//SpriteId sid = PDGG(CTL_SICONS) + (pCastle->Proto()->Size()-1)*CTLT_COUNT + pCastle->Proto()->Type();
	//gGfxMgr.Blit(sid, surf, iPoint(frc.x+2,frc.y+2));
	if (!pCastle->CanBuild()) {
		gGfxMgr.Blit(PDGG(ICN_BUILT), surf, iPoint(frc.x+26,frc.y+14));
	}

	// name
	gTextComposer.TextOut(fc_name, surf, iPoint(), pCastle->Name(), iRect(frc.x+34,frc.y,frc.w-34,frc.h),AlignTop);

	// income
	iMineralSet ms; ms.Reset();
	ms.quant[MINERAL_GOLD] = CTL_INCOME;
	for (uint32 xx=0; xx<pCastle->Cnsts().Count(); ++xx) {
		if (iMineCtlCnst* pMine = DynamicCast<iMineCtlCnst*>(pCastle->Cnst(xx))) {
			if (pMine->IsBuilt()) ms.quant[pMine->m_mt] += pMine->m_income;
		}
	}

	gTextComposer.TextOut(dlgfc_plain, gApp.Surface(), iPoint(), MineralSet2Text(ms), iRect(frc.x+34,frc.y+12,frc.w-34,frc.h),AlignTop);

}

// Game Popup menu
#include "CommonDialogs.h"
#include "GameMenuDlg.h"
#include "Dlg_Save.h"
#include "Dlg_ScenProps.h"
#include "Dlg_PuzzleMap.touch.h"
#include "Dlg_KingdomOverview.touch.h"

enum GameMenuItems {
	GMI_KOVERVIEW,
	GMI_PUZZLEMAP,
	GMI_SETTINGS,
	GMI_GAMEMENU,
	GMI_COUNT
};

const TextResId GameMenuItemText[GMI_COUNT] = {
	TRID_KINGDOM_OVERVIEW,
	TRID_PUZZLE_MAP,
	TRID_MENU_GAMESETTINGS,
	TRID_SYSTEM_MENU
};

/*const SpriteId GameMenuItemIcons[GMI_COUNT] = {
	PDGG(BTN_CASTLE),
	PDGG(BTN_PUZZLEMAP),
	PDGG(BTN_MAINMENU),
	PDGG(BTN_DISK)
};
*/

static void GameMenuHandler(sint32 itemId)
{
	if (itemId == GMI_KOVERVIEW) {
		iKigdomOverviewDlg kodlg(&gApp.ViewMgr(), *gGame.Map().ActPlayer());
		kodlg.DoModal();
	} else if (itemId == GMI_PUZZLEMAP){
		iPuzzleMapDlg pmdlg(&gApp.ViewMgr(), *gGame.Map().ActPlayer(), 0);
		pmdlg.DoModal();
	} else if (itemId == GMI_SETTINGS){
		iSettingsDlg sdlg(&gApp.ViewMgr(), false);
		sdlg.DoModal();
	} else if (itemId == GMI_GAMEMENU){
		iGameMenuDlg gmdlg(&gApp.ViewMgr(), false);
		sint32 res= gmdlg.DoModal();
		if (res == GMRC_SAVE) {
			// Save game
			iSaveDlg sdlg(&gApp.ViewMgr(), true);
			if (sdlg.DoModal() == DRC_OK) {
				iStringT fname;
				if(sdlg.SelFile(fname)) {
					SaveGameFile(fname, gGame.Map());
				} else {
					iTextDlg dlg(&gApp.ViewMgr(), gTextMgr[TRID_MSG_UNABLE_TO_SAVE], gTextMgr[TRID_MSG_NO_SAVE_SLOTS], PID_NEUTRAL);
					dlg.DoModal();
				}

			}
		} else if (res == GMRC_LOAD) {
			// Load game
			iQuestDlg qdlg(&gApp.ViewMgr(), iStringT(), gTextMgr[TRID_CONFIRM_RELOAD], PID_NEUTRAL);
			if (qdlg.DoModal() == DRC_YES) {
				iSaveDlg sdlg(&gApp.ViewMgr(), false);
				if (sdlg.DoModal() == DRC_OK) {
					iMapInfo scenProps = sdlg.SelScenario();
					//iScenPropsDlg spdlg(&gApp.ViewMgr(), scenProps, true);
					//if (spdlg.DoModal() == DRC_OK)	{
						scenProps.ReorderPlayers();
						gGame.ExitGame(false);
						gGame.StartNewGame(scenProps, false, false);
					//}
				} 
			}
		} else if (res == GMRC_RESTART) {
			iQuestDlg qdlg(&gApp.ViewMgr(), iStringT(), gTextMgr[TRID_CONFIRM_RESTART], PID_NEUTRAL);
			if (qdlg.DoModal() == DRC_YES) {
			}
		} else if (res == GMRC_MAINMENU) {
			// Quit to main menu
			iQuestDlg qdlg(&gApp.ViewMgr(), iStringT(), gTextMgr[TRID_CONFIRM_EXITMM], PID_NEUTRAL);
			if (qdlg.DoModal() == DRC_YES) gGame.MainMenu();
		} else if (res == GMRC_QUIT) {
			// Quit to OS
			iQuestDlg qdlg(&gApp.ViewMgr(), iStringT(), gTextMgr[TRID_CONFIRM_QUIT], PID_NEUTRAL);
			if (qdlg.DoModal() == DRC_YES) {				
				if(
#ifdef MULTIPLAYER
				!gMPMgr.IsMultiplayer() &&
#endif
					gGame.Map().CurPlayer()->PlayerType() == PT_HUMAN) {
						iStringT saveDir = gSavePath.Left(gSavePath.Length()-1);
						bool bOk = false;
						if (iFile::DirExists(saveDir) || iFile::DirCreate(saveDir)) {
							iFilePtr pFile(CreateWin32File(gSavePath + _T("lastses.tmp")));
							check(pFile);
							bOk = (pFile && gGame.Map().SaveToFile(pFile.operator->()));
						}
						if (bOk) iFile::Move(gSavePath + _T("lastses.tmp"), gSavePath + _T("lastses.phsd"));					
				}
				gGame.Quit();
			}
		} else if (res == GMRC_RETURN) {
			// do nothing
		} else {
			check(0 == "Unknown menu item?");
		}
	} 
}

iGamePopupMenu::iGamePopupMenu(iViewMgr* pViewMgr)
: iPopupList(pViewMgr, GameMenuHandler, iSize(120,18), GMI_COUNT, -1, 15)
{}

void iGamePopupMenu::DrawItem(sint32 itemId, iDib& surf, const iRect& rc, uint32 flags)
{
	//ButtonFrame(surf,rc,0 );
	iRect frc(rc);
	//if (flags & iButton::Selected) {
		gGfxMgr.BlitTile(PDGG(CTILE), surf,frc);
		ButtonFrame(surf,frc,0);
	//}
	frc.DeflateRect(1);

	// icon
	/*BlitIcon(surf, GameMenuItemIcons[itemId], iRect(frc.x, frc.y, 18, frc.h));*/
	frc.DeflateRect(18,0,0,0);

	// text
	iTextComposer::FontConfig fc_name(FS_MEDIUM, RGB16(220,220,220));
	gTextComposer.TextOut(fc_name, surf, iPoint(), gTextMgr[GameMenuItemText[itemId]], frc,AlignLeft);
}

void iGlowTextButton::OnCompose()
{
	ComposeGlowTextButton(gApp.Surface(),GetScrRect(),GetButtonState(),m_TextKey);
}

iGlowTextButton::iGlowTextButton( iViewMgr* pViewMgr, IViewCmdHandler* pCmdHandler, const iRect& rect, TextResId textKey, uint32 uid, uint32 state /*= Visible|Enabled*/ ):
	iButton(pViewMgr, pCmdHandler, rect, uid, state), m_TextKey(textKey)
{

}
