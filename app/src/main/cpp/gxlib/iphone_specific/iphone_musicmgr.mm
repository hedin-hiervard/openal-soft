/*
 *  musicmgr.mm
 *  pheroes
 *
 *  Created by hedin on 5/19/10.
 *  Copyright 2010 Palm Heroes Team. All rights reserved.
 *
 */

#include <Foundation/NSObject.h>
#include <Foundation/NSError.h>
#include <Foundation/NSTimer.h>
#include <Foundation/NSURL.h>
//#include <AVAudioPlayer+PGFade.h>

#import <AVFoundation/AVAudioPlayer.h>

#import <UIKit/UIKit.h>
#import <Foundation/NSThread.h>


AVAudioPlayer* player = NULL;

double volume = 1.0;

const double fadeDuration = 1.0f;

void iphoneMusicMgr_SetVolume(double newvol)
{
    volume = newvol;
    if(player) {
        if([player isPlaying])
            [player setVolume: newvol];
    }
}

bool iphoneMusicMgr_Init()
{
	return true;
}

void iphoneMusicMgr_Play(const char *filename)
{    
    if(!player) {
        NSError *error;
        player = [AVAudioPlayer alloc];
        [player initWithContentsOfURL: [[NSURL alloc] initFileURLWithPath: [NSString stringWithUTF8String: filename]] error: &error];
        [player setVolume: volume];
        [player setNumberOfLoops: -1];
        [player play];
    } else {
        NSError *error;
        NSString *path = [NSString stringWithUTF8String: filename];
        [player stop];
        [player initWithContentsOfURL: [[NSURL alloc] initFileURLWithPath: path] error: &error];
        [player setNumberOfLoops: -1];
        [player setVolume: volume];
        [player play];
        
        // disabled
        
//        __block NSString *path = [NSString stringWithUTF8String: filename];
//        [player fadeToVolume: 0.0 duration: fadeDuration setPos: false completion: ^
//         {
//             NSError *error;
//             [player stop];
//             [player initWithContentsOfURL: [[NSURL alloc] initFileURLWithPath: path] error: &error];
//             [player setNumberOfLoops: -1];
//             [player setVolume: 0];
//             [player fadeToVolume: volume duration: fadeDuration  setPos: true ];
//         }];            
    }
}

void iphoneMusicMgr_Stop() 
{
    if(player)
        [player stop];
//        [player stopWithFadeDuration: fadeDuration];
}

void iphoneMusicMgr_Deinit()
{
}


unsigned int iphoneMusicMgr_GetCurPos()
{
    if(player)
        return player.currentTime;
    else {
        return 0;
    }
}

void iphoneMusicMgr_SetCurPos(unsigned int pos)
{
    if(player)
        player.currentTime = pos;
//        [player setFadeTargetPos: pos];
}

