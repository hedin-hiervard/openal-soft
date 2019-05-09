#ifndef GXLIB_IPHONE_SPECIFIC_IPHONE_MUSICMGR_H_
#define GXLIB_IPHONE_SPECIFIC_IPHONE_MUSICMGR_H_
/*
 *  musicmgr.h
 *  pheroes
 *
 *  Created by hedin on 5/19/10.
 *  Copyright 2010 Palm Heroes Team. All rights reserved.
 *
 */

bool iphoneMusicMgr_Init();
void iphoneMusicMgr_Play(const char *filename);
void iphoneMusicMgr_Stop();
void iphoneMusicMgr_SetVolume(double newvol);
void iphoneMusicMgr_Deinit();
unsigned int iphoneMusicMgr_GetCurPos();
void iphoneMusicMgr_SetCurPos(unsigned int pos);
#endif //GXLIB_IPHONE_SPECIFIC_IPHONE_MUSICMGR_H_
