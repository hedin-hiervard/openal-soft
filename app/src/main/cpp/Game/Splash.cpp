/*
 *  Splash.cpp
 *  pheroes
 *
 *  Created by hedin on 6/9/10.
 *  Copyright 2010 Palm Heroes Team. All rights reserved.
 *
 */

#include "stdafx.h"
#include "Splash.h"

#include "xau/xjp.codec.h"

void iSplash::Show() {
	iFilePtr sdataFile = OpenWin32File(gDataPath + L"game.pix");
	if (!sdataFile) return;
	
	uint32 tmpBuffLen;
	
	xjp::decoder_t dec;
	void* tmpBuff;
	
	back.Init(iSize(480, 320), iDib::RGB);
	
	sdataFile->Read(&tmpBuffLen, sizeof(tmpBuffLen));	
	tmpBuff = malloc( tmpBuffLen );
	sdataFile->Read(tmpBuff, tmpBuffLen);
	
	xjp::dec_init( &dec, NULL, tmpBuff );
	
	xjp::dec_do( &dec, back.GetPtr(), 2 );
	xjp::dec_done( &dec );
	
	free( tmpBuff ); 

	uint32 startticks = GetTickCount();
	uint32 curtick;
	uint32 len = 1000;
	while((curtick = GetTickCount()) - startticks < len) {
		back.CopyRectToDibXY(gApp.Surface(), iRect(0, 0, 480, 320), iPoint());
		gApp.Surface().FillRect(gApp.Surface().GetSize(), cColor_Black, 255-(255*(curtick-startticks)/len));
		gApp.Cycle();

	}
}

void iSplash::Disappear() {
	uint32 startticks = GetTickCount();
	uint32 curtick;
	uint32 len = 1000;
	while((curtick = GetTickCount()) - startticks < len) {
		back.CopyRectToDibXY(gApp.Surface(), iRect(0, 0, 480, 320), iPoint());
		gApp.Surface().FillRect(gApp.Surface().GetSize(), cColor_Black, (255*(curtick-startticks)/len));
		gApp.Cycle();		
	}
}