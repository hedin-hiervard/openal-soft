#include "stdafx.h"
#include "CastleView.h"

////////////////	//////////////////////////////////////////////////////////
///*iBaseGameDlg::iBaseGameDlg(iViewMgr* pViewMgr)
//: iDialog(pViewMgr)
//{}
//
//iRect iBaseGameDlg::ClientRect() const
//{
//	iRect rc = GetDlgMetrics();
//	rc.DeflateRect(DLG_FRAME_SIZE);
//	return rc;
//}
//
//iSize iBaseGameDlg::GetMaxClientSize() const
//{
//	iSize res = gApp.Surface().GetSize();
//	res.w -= DLG_FRAME_SIZE * 2;
//	res.h -= DLG_FRAME_SIZE * 2;
//	return res;
//}
//
//void iBaseGameDlg::OnCompose()
//{
//	ComposeDlgBkgnd(gApp.Surface(), m_Rect, PID_NEUTRAL, true);
//	iRect rc(m_Rect);
//	rc.DeflateRect(DLG_FRAME_SIZE);
//	DoCompose(rc);
//}
//
//iSize iBaseGameDlg::GetDlgMetrics() const
//{
//	iSize siz = ClientSize();
//	iSize max = gApp.Surface().GetSize();
//	siz.w = iCLAMP<uint32>(80, max.w - 6, siz.w+DLG_FRAME_SIZE*2);
//	siz.h = iCLAMP<uint32>(40, max.h - 6, siz.h+DLG_FRAME_SIZE*2);
//	return siz;
//}
//
//iRect iBaseGameDlg::AlignChild( iSize sz, Alignment al, sint32 offsetFromTop /*= 0*/ )
//{
//	iRect clrc = GetDlgMetrics();
//	clrc.x += DLG_FRAME_SIZE;
//	clrc.y += DLG_FRAME_SIZE;
//	clrc.DeflateRect(DLG_FRAME_SIZE, DLG_FRAME_SIZE);
//	clrc.DeflateRect(0, offsetFromTop, 0, 0);
//	return AlignRect(sz, clrc, al) - ClientRect();
//}


//////////////////////////////////////////////////////////////////////////
iGlowGameDlg::iGlowGameDlg(iViewMgr* pViewMgr)
: iDialog(pViewMgr)
{}

iRect iGlowGameDlg::ClientRect() const
{
    iRect rc = GetDlgMetrics();
    if(gUseIpadUI)
        rc.DeflateRect(DLG_GLOW_FRAME_SIZE);
    
    return rc;
}

iSize iGlowGameDlg::GetMaxClientSize() const
{
    iSize res = gApp.Surface().GetSize();
    if(gUseIpadUI)
    {
        res.w -= DLG_GLOW_FRAME_SIZE;
        res.h -= DLG_GLOW_FRAME_SIZE;
    }
    
    return res;
}

void iGlowGameDlg::OnCompose()
{
    bool frame = gUseIpadUI;
    if(!frame)
    {
        gApp.Surface().FillRect(iRect(iPoint(), gApp.Surface().GetSize()), cColor_Black);
        gGfxMgr.Blit(PDGG(HERO_DIALOG_BACK), gApp.Surface(), AlignRect(gGfxMgr.Dimension(PDGG(HERO_DIALOG_BACK)), GetScrRect(), AlignCenter));
    }
    else
    {
        // darken the underlying image
        //gApp.Surface().DarkenBWRect(iRect(0, 0, gApp.Surface().GetWidth(), gApp.Surface().GetHeight()));
        if(gGame.ActViewType() != iChildGameView::CV_CASTLE) {
            gApp.Surface().Darken50Rect(iRect(0, 0, gApp.Surface().GetWidth(), gApp.Surface().GetHeight()));
            gApp.Surface().Darken50Rect(iRect(0, 0, gApp.Surface().GetWidth(), gApp.Surface().GetHeight()));
        }
        
        ComposeGlowBkgnd(gApp.Surface(), m_Rect, gGame.ActViewType() == iChildGameView::CV_OVERLAND || gGame.ActViewType() == iChildGameView::CV_BATTLE);
    }
    iRect rc(m_Rect);
    if(frame)
    {
        rc.DeflateRect(DLG_GLOW_FRAME_SIZE);
    }
    
    DoCompose(rc);
}

iSize iGlowGameDlg::GetDlgMetrics() const
{
    iSize siz = ClientSize();
    iSize max = gApp.Surface().GetSize();
    if(gUseIpadUI)
    {
        siz.w = iCLAMP<uint32>(80, max.w - 6, siz.w + 2 * DLG_GLOW_FRAME_SIZE);
        siz.h = iCLAMP<uint32>(40, max.h - 6, siz.h + 2 * DLG_GLOW_FRAME_SIZE);
    }
    return siz;
}

iRect iGlowGameDlg::AlignChild( iSize sz, Alignment al, sint32 offsetFromTop /*= 0*/ )
{
    iRect clrc = GetDlgMetrics();
    clrc.h = iMIN(gApp.Surface().GetHeight(), clrc.h);
    clrc.w = iMIN(gApp.Surface().GetWidth(), clrc.w);
    if(gUseIpadUI)
    {
        clrc.x += DLG_GLOW_FRAME_SIZE;
        clrc.y += DLG_GLOW_FRAME_SIZE;
        clrc.DeflateRect(DLG_GLOW_FRAME_SIZE / 2, DLG_GLOW_FRAME_SIZE / 2);
    }
    clrc.DeflateRect(0, offsetFromTop, 0, 0);
    return AlignRect(sz, clrc, al) - ClientRect();
}

sint32 iGlowGameDlg::GetTopOffset()
{
    if(gUseIpadUI) return 15;
    else return 5;
}

sint32 iGlowGameDlg::GetDefaultWidth()
{
    if(!gUseIpadUI)
        return gApp.Surface().GetWidth() - 20;
    else
    {
        uint32 width = gApp.Surface().GetWidth() / 3;
        
        if( width < 700 )
            width = 700;
        
        return width;
    }
}


//////////////////////////////////////////////////////////////////////////
iTextDlg::iTextDlg(iViewMgr* pViewMgr, const iStringT& title, const iStringT& text, PLAYER_ID pid, iTextComposer::FontConfig fc_title, iTextComposer::FontConfig fc_text)
: iBaseGameDlg(pViewMgr), m_title(title), m_text(text), m_fcTitle(fc_title), m_fcText(fc_text)
{}


void iTextDlg::OnCreateDlg()
{
#ifdef MULTIPLAYER
    if(gMPMgr.IsReplaying()) {
        EndDialog(DRC_OK);
        return;
    }
#endif
    iRect clRect = ClientRect();
    //AddChild(new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-20),clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT),TRID_OK, DRC_OK));
}

void iTextDlg::SetText(const iStringT& text)
{
    m_text = text;
}

void iTextDlg::DoCompose(const iRect& clRect)
{
    iRect rc(clRect);
    
    iSize titlesz = gTextComposer.GetTextBoxSize(m_title, rc.w, m_fcTitle);
    iSize textsz = gTextComposer.GetTextBoxSize(m_text, rc.w, m_fcText);
    iRect allrc = iRect(0, 0, rc.w, titlesz.h + textsz.h + 20);
    rc = AlignRect(allrc.size(), rc, AlignCenter);
    
    if (!m_title.Empty()) {
        sint32 h = gTextComposer.TextBoxOut(m_fcTitle, gApp.Surface(), m_title, rc);
        rc.y += h;
        rc.y += 10;
    }
    
    if (!m_text.Empty()) {
        sint32 h = gTextComposer.TextBoxOut(m_fcText, gApp.Surface(), m_text, rc);
        rc.y += h;
        h += 10;
    }
}

iSize iTextDlg::ClientSize() const
{
    if(!gUseIpadUI)
    {
        return gApp.Surface().GetSize();
    }
    
    sint32 w = iBaseGameDlg::GetDefaultWidth();
    
    sint32 h = iBaseGameDlg::GetTopOffset();
    
    // title
    if (!m_title.Empty()) {
        iSize s = gTextComposer.GetTextBoxSize(m_title, w, m_fcTitle);
        h += s.h;
        h += 50;
    }
    
    // text
    if (!m_text.Empty()) {
        iSize s = gTextComposer.GetTextBoxSize(m_text, w, m_fcText);
        h += s.h;
        h += 10;
    }
    
    // ok button
    //h += DEF_BTN_HEIGHT;
    
    return iSize(w
#ifdef PC_VERSION
                 + 50
#endif
                 , h);
}

void iTextDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
    uint32 uid = pView->GetUID();
    EndDialog(uid);
}

void iTextDlg::OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID)
{
    EndDialog(0);
}
//////////////////////////////////////////////////////////////////////////
iExtTextDlg::iExtTextDlg(iViewMgr* pViewMgr, const iStringT& title, const iStringT& text, const iStringT& sign, PLAYER_ID pid)
: iBaseGameDlg(pViewMgr), m_title(title), m_text(text), m_sign(sign)
{}


void iExtTextDlg::OnCreateDlg()
{
    iRect clRect = ClientRect();
    AddChild(m_pBtnOk = new iTextButton(m_pMgr,this,iRect(clRect.x+(clRect.w/2-20),clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT),TRID_OK, DRC_OK));
}
#ifdef PC_VERSION
void iExtTextDlg::UpdateSize(){
    
    iRect clRect = ClientRect();
    
    m_pBtnOk->SetRect( iRect(clRect.x+(clRect.w/2-20),clRect.y2()-DEF_BTN_HEIGHT,40,DEF_BTN_HEIGHT) );
}
#endif
void iExtTextDlg::DoCompose(const iRect& clRect)
{
    iRect rc(clRect);
    
    if (!m_title.Empty()) {
        sint32 h = gTextComposer.TextBoxOut(dlgfc_hdr, gApp.Surface(), m_title, rc);
        rc.y += h;
        rc.y += 10;
    }
    
    if (!m_text.Empty()) {
        sint32 h = gTextComposer.TextBoxOut(dlgfc_plain, gApp.Surface(), m_text, rc);
        rc.y += h;
        h += 10;
    }
    
    if (!m_sign.Empty()) {
        rc.y += 5;
        sint32 h = gTextComposer.TextBoxOut(dlgfc_topic, gApp.Surface(), m_sign, rc);
        rc.y += h;
        h += 5;
    }
}

iSize iExtTextDlg::ClientSize() const
{
    sint32 w = 130;
    if (m_text.Length() > 20) w += iCLAMP<sint32>(0,70,m_text.Length()/4);
    sint32 h = 0;
    
    // title
    if (!m_title.Empty()) {
        iSize s = gTextComposer.GetTextBoxSize(m_title, w, dlgfc_hdr);
        h += s.h;
        h += 10;
    }
    
    // text
    if (!m_text.Empty()) {
        iSize s = gTextComposer.GetTextBoxSize(m_text, w, dlgfc_plain);
        h += s.h;
        h += 10;
    }
    
    // sign
    if (!m_text.Empty()) {
        iSize s = gTextComposer.GetTextBoxSize(m_sign, w, dlgfc_topic);
        h += s.h;
        h += 10;
    }
    
    // ok button
    h += DEF_BTN_HEIGHT;
    
    return iSize(w, h);
}

void iExtTextDlg::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
{
    uint32 uid = pView->GetUID();
    EndDialog(uid);
}
//////////////////////////////////////////////////////////////////////////
iIconDlg::iIconDlg(iViewMgr* pViewMgr, const iStringT& title, const iStringT& text, SpriteId sid, PLAYER_ID pid)
: iTextDlg(pViewMgr, title, text, pid), m_sid(sid)
{}

void iIconDlg::DoCompose(const iRect& clRect)
{
    iRect rc(clRect);
    iTextDlg::DoCompose(clRect);
    iSize ssiz = gGfxMgr.Dimension(m_sid);
    iPoint sa = gGfxMgr.Anchor(m_sid);
    rc.y = rc.y2() - 15 - ssiz.h;
    rc.x = rc.x + (rc.w/2 - ssiz.w/2);
    gGfxMgr.Blit(m_sid,gApp.Surface(),rc.point()-sa);
}

iSize iIconDlg::ClientSize() const
{
    iSize res = iTextDlg::ClientSize();
    iSize ssiz = gGfxMgr.Dimension(m_sid);
    res.h += ssiz.h + 10;
    res.w = iMIN<sint32>(gApp.Surface().GetWidth(), res.w);
    res.h = iMIN<sint32>(gApp.Surface().GetHeight(), res.h);
    return res;
}
//////////////////////////////////////////////////////////////////////////
iQuestDlg::iQuestDlg(iViewMgr* pViewMgr, const iStringT& title, const iStringT& text, PLAYER_ID pid)
: iTextDlg(pViewMgr, title, text, pid)
{}

iSize iQuestDlg::ClientSize() const
{
    iSize res = iTextDlg::ClientSize();
    res.h +=
#ifdef OS_IPHONE
    DEF_BTN_HEIGHT;
#else
    2*DEF_BTN_HEIGHT;
#endif
    res.w = iMIN<sint32>(gApp.Surface().GetWidth(), res.w);
    res.h = iMIN<sint32>(gApp.Surface().GetHeight(), res.h);
    
    return res;
}

void iQuestDlg::OnCreateDlg()
{
    iRect clRect = ClientRect();
    
    //sint32 bw = iMAX<>(gTextComposer.GetTextSize(gTextMgr[TRID_YES], btnfc_normal).w, gTextComposer.GetTextSize(gTextMgr[TRID_NO], btnfc_normal).w) + 30;
    //	AddChild(m_pBtnYes = new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) + iPoint(bw, 0),TRID_YES, DRC_YES));
    //	AddChild(m_pBtnNo = new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) - iPoint(bw, 0),TRID_NO, DRC_NO));
    sint32 bw = iIphoneBtn::sm_defSize.w;
    sint32 bh =
#ifdef OS_IPHONE
    0;
#else
    iIphoneBtn::sm_defSize.h;
#endif
    AddChild(m_pBtnYes = new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom) + iPoint(bw, -bh), DRC_YES, PDGG(BTN_OK_DELUXE), -1));
    AddChild(m_pBtnNo = new iImgBtn(&gApp.ViewMgr(), this, AlignChild(iIphoneBtn::sm_defSizeDeluxe, AlignBottom) + iPoint(-bw, -bh), DRC_NO, PDGG(BTN_CANCEL_DELUXE), -1));
    
    //    AddChild(m_pBtnYes = new iImgBtn(&gApp.ViewMgr(), this, iRect(iPoint(), iIphoneBtn::sm_defSize), DRC_YES, PDGG(BTN_OK_DELUXE), -1));
    
    //    AddChild(m_pBtnYes = new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) - iPoint(bw, 0));
    //    AddChild(m_pBtnNo = new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) - iPoint(bw, 0),TRID_NO, DRC_NO));
}
#ifdef PC_VERSION
void iQuestDlg::UpdateSize(){
    
    Center();
    
    sint32 bw = iMAX<>(gTextComposer.GetTextSize(gTextMgr[TRID_YES], btnfc_normal).w, gTextComposer.GetTextSize(gTextMgr[TRID_NO], btnfc_normal).w) + 30;
    
    m_pBtnYes->SetRect( AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) + iPoint(bw, 0) );
    m_pBtnNo->SetRect( AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) - iPoint(bw, 0) );
}
#endif
//////////////////////////////////////////////////////////////////////////
iPursueDlg::iPursueDlg(iViewMgr* pViewMgr, const iStringT& title, const iStringT& text, PLAYER_ID pid)
: iTextDlg(pViewMgr, title, text, pid)
{}

iSize iPursueDlg::ClientSize() const
{
    iSize res = iTextDlg::ClientSize();
    res.h += DEF_BTN_HEIGHT;
    res.w = iMIN<sint32>(gApp.Surface().GetWidth(), res.w);
    res.h = iMIN<sint32>(gApp.Surface().GetHeight(), res.h);
    
    return res;
}

void iPursueDlg::OnCreateDlg()
{
    iRect clRect = ClientRect();
    sint32 bw = gTextComposer.GetTextSize(gTextMgr[TRID_CFGENTRY_QUICKCOMBAT], btnfc_normal).w + 30;
    AddChild(m_pBtnYes = new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) + iPoint(bw, 0),TRID_YES, DRC_YES));
    if(!gSettings.GetEntryValue(CET_QUICKCOMBAT))
        AddChild(m_pBtnQuick = new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom),TRID_CFGENTRY_QUICKCOMBAT, DRC_QUICK));
    AddChild(m_pBtnNo = new iTextButton(m_pMgr,this,AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) - iPoint(bw, 0),TRID_NO, DRC_NO));
}
#ifdef PC_VERSION
void iPursueDlg::UpdateSize(){
    
    Center();
    
    sint32 bw = gTextComposer.GetTextSize(gTextMgr[TRID_CFGENTRY_QUICKCOMBAT], btnfc_normal).w + 30;
    
    m_pBtnYes->SetRect( AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) + iPoint(bw, 0) );
    
    if(!gSettings.GetEntryValue(CET_QUICKCOMBAT))
        m_pBtnQuick->SetRect( AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) );
    
    m_pBtnNo->SetRect( AlignChild(iSize(bw, DEF_BTN_HEIGHT), AlignBottom) - iPoint(bw, 0) );
}
#endif
