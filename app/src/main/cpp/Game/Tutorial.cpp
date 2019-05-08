//
//  Tutorial.cpp
//  pheroes
//
//  Created by Andrey Kiryushkin on 1/30/12.
//  Copyright (c) 2012 Palm Heroes Team. All rights reserved.
//

#include "stdafx.h"
#include "Dlg_Tutorial.h"
#include "FingerHelp.h"

void iTutorial::Trigger(TUT_MSG msg)
{
   if(!m_bDisabled[msg] && gGame.Map().CurPlayer() && gGame.Map().CurPlayer()->PlayerType() == PT_HUMAN) {
        
        const sint16 msgcntrls = TRID_TUT_MAP_CONTROLS;
        iDlg_Tutorial dlg(&gApp.ViewMgr(), msgcntrls + msg, msg);
        dlg.DoModal();    
    }
}
    
void iTutorial::MarkDontShow(TUT_MSG msg)
{
    m_bDisabled[msg] = true;
    Save();
}

void iTutorial::MarkDontShowAll(){

    
    for( uint32 xx=0; xx<TUT_COUNT; xx++ ){
    
        m_bDisabled[xx] = true;
        Save();
    }        
}

void iTutorial::Save()
{    
    iFileStream pConfigFile(CreateWin32File(gTutorialCfgFilePath));	
	if (!pConfigFile.IsValid()) return;
    
    for(uint32 xx=0; xx<TUT_COUNT; xx++) {
        pConfigFile.Write((uint8)m_bDisabled[xx]);
    }
}

void iTutorial::ResetDontShow()
{
    for(uint32 xx=0; xx<TUT_COUNT; xx++)
        m_bDisabled[xx] = false;
    Save();
}

void iTutorial::Init()
{
    for(uint32 xx=0; xx<TUT_COUNT; xx++)
        m_bDisabled[xx] = false;
    
    iFileStream pConfigFile(OpenWin32File(gTutorialCfgFilePath));	
	if (!pConfigFile.IsValid()) return;
	
    for(uint32 xx=0; xx<TUT_COUNT; xx++) {
        uint8 tmp = false;
        pConfigFile.Read(tmp);
        m_bDisabled[xx] = tmp;
    }

}
    
void iTutorial::ShowFingerHelp()
{
#ifdef PLAYCITY
    if(gSettings.LaunchCount() == 0) {
        iFingerHelp fh(&gApp.ViewMgr(), FH_LEFT | FH_RIGHT | FH_UP);
        fh.DoModal();
    }
#endif
}