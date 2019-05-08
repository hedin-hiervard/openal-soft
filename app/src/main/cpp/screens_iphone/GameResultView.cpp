#include "stdafx.h"
#include "GameResultView.h"

namespace UI_IPHONE
{
    
    const uint32 TPANE_OFFSET = 250;
    
    iGameResultView::iGameResultView() :
    IGameResultView(false, CV_MENU),
    m_textGrad(0)
    {
        AddChild(m_pTPane = new iTwitterPane(&gApp.ViewMgr(), this, iRect(), 100));
    }
    
    //void iTextComposer::TextOut(iDib& dib, const iPoint& pos, const iStringT& text, const iRect& rect, Alignment al, const iPoint& offset)
    
    
    void iGameResultView::OnCompose()
    {
        sint16 sid = m_type == GRWin ? PDGG(WIN_BACKGROUND) : PDGG(DEFEAT_BACKGROUND);
        iRect brc = AlignRect(gGfxMgr.Dimension(sid), m_Rect, AlignCenter);
        gApp.Surface().Fill(cColor_Black);
        gGfxMgr.Blit(sid, gApp.Surface(), brc.point());
        iTextComposer::FontConfig fc(grfc_tap);
        //	fc.cmpProps.faceColor = (m_textGrad - 128) * (cColor_White - fc.cmpProps.faceColor) / 128 + fc.cmpProps.faceColor;
        sint16 add = m_textGrad < 125 ? m_textGrad : 250 - m_textGrad;
        fc.cmpProps.faceColor = RGB16(100 + add, 100 + add, 100 + add);
        
        iRect rc(0, 0, m_Rect.w, m_Rect.h - 5);
        gTextComposer.TextOut(fc, gApp.Surface(), iPoint(), iStringT(_T("Tap to continue...")), rc, AlignBottom);
        
        uint8 alpha = iCLAMP<sint32>(0, 255, (m_Rect.w - iTwitterPane::sm_defSize.w + TPANE_OFFSET - m_pTPane->GetPos().x) * 255 / 60);
        alpha = 255 - alpha;
        
        if(!m_pTPane->IsVisible() ||
           ((m_pTPane->GetRect().x2() >= m_Rect.w))) {
            iPoint offs;
            
            if(m_pTPane->IsVisible()) {
                offs = iPoint(-gGfxMgr.Dimension(PDGG(TWEET)).w, 0);
            }
            iRect rc = AlignRect(gTextComposer.GetTextSize(m_title, grfc_title), m_Rect, AlignRight) + offs;
            if(rc.x2() > m_Rect.w) {
                rc.x -= (rc.x2() - m_Rect.w);
            }
            iTextComposer::FontConfig fc(grfc_title);
            fc.fontSize--;
            fc.cmpProps.alpha = alpha;
            gTextComposer.TextOut(fc, gApp.Surface(), iPoint(), m_title, rc, AlignCenter);
            iSize sz = gTextComposer.GetTextSize(m_text, fc);
            rc = AlignRect(sz, m_Rect, AlignRight) + iPoint(0, 30);
            fc = grfc_text;
            fc.cmpProps.alpha = alpha;
#ifndef ROYAL_BOUNTY
            gTextComposer.TextBoxOut(fc, gApp.Surface(), m_text, rc + iPoint(0, 30));
#endif
        }
    }
    
#ifdef ROYAL_BOUNTY
    const iCharT* tweets[5] = {
        _T("Thank God I've finished it!"),
        _T("Just finished Royal Bounty. I think it's boring."),
        _T("Completed Royal Bounty. So-so."),
        _T("Finished Royal Bounty, one of the best things I've played!"),
        _T("Wow! We have completed Royal Bounty, and we are heroes!")
    };
#else
    const iCharT* tweets[5] = {
        _T("I finished the map %s. I think it's horrible."),
        _T("I just finished the map %s. I didn't quite like it."),
        _T("Completed the map %s. It's ok, but some others are better."),
        _T("Finished the map %s, it's quite good."),
        _T("Just played the map %s, absolutely awesome!")
    };
#endif
    
    void iGameResultView::OnMouseDown(const iPoint& pos, MouseId mID, MouseButtonId mbID)
    {
        iRect rc(0, m_Rect.h - 400, m_Rect.w, 400);
        if(!rc.PtInRect(pos)) return;
        
        if(m_bTweet) {
            iStringT text = iFormat(tweets[m_pTPane->SelectedRating()], m_mapName.CStr());
        }
        gGame.MainMenu();
    }
    
    void iGameResultView::UpdateSize()
    {
        m_pTPane->SetRect(AlignChild(iTwitterPane::sm_defSize, AlignRight) + iPoint(TPANE_OFFSET, 0));
        m_pTPane->UpdateSize();
    }
    
    void iGameResultView::iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param)
    {
        uint32 uid = pView->GetUID();
        if(uid == 100) {
            if(cmd == CCI_BTNCLICK) {
                gAniHost.StopAllAnimation(false);
                iRect rc = AlignChild(iTwitterPane::sm_defSize, AlignRight) + iPoint(TPANE_OFFSET, 0);
                if(m_pTPane->RatingAlpha() == 0) {
                    rc.x -= TPANE_OFFSET;
                    AniQID q = gAniHost.AddAniObj(new iGAniObj_Move(fix32(1.0f), m_pTPane->MovableRect(), rc));
                    gAniHost.AddAniObjToQ(new iGAniObj_ChangeVal<uint8>(fix32(1.0f), m_pTPane->RatingAlpha(), 255), q);
                } else {
                    AniQID q = gAniHost.AddAniObj(new iGAniObj_ChangeVal<uint8>(fix32(1.0f), m_pTPane->RatingAlpha(), 0));
                    gAniHost.AddAniObjToQ(new iGAniObj_Move(fix32(1.0f), m_pTPane->MovableRect(), rc), q);
                }
            } else if(cmd == CCI_LBSELECTED) {
                m_bTweet = true;
            }
        }
    }
    
    void iGameResultView::OnActivate(bool bActivate)
    {
        if(bActivate) {
            m_bTweet = false;
            m_pTPane->ShowRating();
            m_pTPane->RatingAlpha() = 0;
            
            
            m_pTPane->SetVisible(m_type == GRWin);
#ifdef ROYAL_BOUNTY
            if(m_type == GRWin)
            {
                iStringT track = gMusicPath + _T("Evil March.mp3");
                gMusicMgr.Play(track, false);
            }
#endif		
        }
    }
    
}
