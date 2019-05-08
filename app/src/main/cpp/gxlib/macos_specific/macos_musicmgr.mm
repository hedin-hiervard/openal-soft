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
#include <AppKit/NSSound.h>
#include <Foundation/NSLock.h>
#include <Foundation/NSAutoreleasePool.h>
//#include <AVFoundation/AVAudioPlayer.h>

//#import <UIKit/UIKit.h>
#import <Foundation/NSThread.h>

#import "iTunes.h"

NSURL *nextTrack = NULL;
double volume = 1.0;

NSTimeInterval posToSet = 0.0;
NSLock* mutex = [NSLock new];
NSLock* _mutex = [NSLock new];
NSLock* playerMutex = [NSLock new];
NSLock* trackMutex = [NSLock new];
NSLock* flagMutex = [NSLock new];
bool resetFlag = false;
NSSound* player =  nil;

iTunesApplication* iTunes = nil;

@interface MusicFader: NSObject {
    
}
- (void) fadeIn: (id) obj;
- (void) fadeOut: (id) obj;
- (void) switchTracks: (id) obj;
- (void) tracks: (id) obj;
@end

@implementation MusicFader

- (void) fadeIn: (id) obj;
{
//    if( volume == 0 )
//        return;
    
	[mutex lock];
   
	while( player && [player volume] < volume) {
/*        [flagMutex lock];
		if(resetFlag) {
			resetFlag = false;
			[flagMutex unlock];
            [mutex unlock];
            return;
		}
		[flagMutex unlock];
*/		double vol = [player volume] + volume /10;
		[player setVolume:vol];
		[NSThread sleepForTimeInterval: 0.1];
	}

	[mutex unlock];
}

- (void) fadeOut: (id) obj;
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	[mutex lock];
	while([player volume] > 0.0) {
		[flagMutex lock];
		if(resetFlag) {
			resetFlag = false;
			[flagMutex unlock];
			[mutex unlock];
			return;
		}
		[flagMutex unlock];
		double vol = [player volume] - volume /10;
		[player setVolume:vol];
		[NSThread sleepForTimeInterval: 0.1];
	}
	[mutex unlock];
	[self switchTracks: nil];
	[pool release];
}

- (void) switchTracks: (id) obj
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	[flagMutex lock];
	if(resetFlag) {
		resetFlag = false;
		[flagMutex unlock];
		[mutex unlock];
		return;
	}
	[flagMutex unlock];
	
	[mutex lock];
    if(player){
        
//        [player setVolume:0.0];
        [player stop];
        [player release];
        player = NULL;
    }   
    
    player = [NSSound alloc];

	[trackMutex lock];
    if(!nextTrack) {
        [trackMutex unlock];
        [mutex unlock];
        [player release];
        player = NULL;
        return;
    }
  
	[player initWithContentsOfURL:nextTrack byReference:YES];
	[player setCurrentTime:posToSet];
	posToSet = 0.0;
	[nextTrack release];
	nextTrack = NULL;
	[trackMutex unlock];
	[player setLoops:YES];
//	player.numberOfLoops = -1;
    [player setVolume:0.0];
	[player stop];
	[player play];
    [mutex unlock];
 //   if( volume > 0 )
        [self fadeIn: nil];
    [pool release];
}

- (void) tracks:(id)obj{

    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    if(player){
        
        [player stop];
        [player release];
        player = NULL;
    }
    
    player = [NSSound alloc];
    
    if(!nextTrack){
     
        [player stop];
        [player release];
        player = NULL;        
        return;
    }
        
    [player initWithContentsOfURL:nextTrack byReference:NO];
    [nextTrack release];
	nextTrack = NULL;
    [player setLoops:YES];
    [player stop];
	[player play];
    [player setVolume:volume];

    [pool release];
}

@end


MusicFader* fader;

void iphoneMusicMgr_SetVolume(double newvol)
{    
	[playerMutex lock];
	if(player)
		[player setVolume:newvol];

	[playerMutex unlock];
	volume = newvol;	
}

bool iphoneMusicMgr_Init()
{
	fader = [MusicFader alloc];
    iTunes = [SBApplication applicationWithBundleIdentifier:@"com.apple.iTunes"];
	return true;
}

void iphoneMusicMgr_Stop() 
{
	[playerMutex lock];
	if([player isPlaying])
		[player stop];
	[playerMutex unlock];
}


void iphoneMusicMgr_Play(const char *filename)
{
    if( [iTunes isRunning] && iTunesEPlSPlaying == [iTunes playerState]){
    
        iphoneMusicMgr_Stop();
        return;
    }
    
	[trackMutex lock];
    nextTrack = [[NSURL alloc] initFileURLWithPath: [NSString stringWithUTF8String: filename]];
	[trackMutex unlock];
    
//    if( volume == 0 )
//        return;
    
    NSLog(@"%s\n", filename);
    
//    [fader tracks:nil];
 
//    [NSThread detachNewThreadSelector:@selector(tracks:)
//                             toTarget:fader
//                           withObject:nil];
    
	
	// nothing is faded right now, we can start new thread
	if([mutex tryLock]) {
		[mutex unlock];
		if([player isPlaying])
			[NSThread detachNewThreadSelector:@selector(fadeOut:)
												  toTarget:fader
                                                  withObject:nil];
		else
			[NSThread detachNewThreadSelector:@selector(switchTracks:)
								 toTarget:fader
							   withObject:nil];
	} else {
		// set the return flag first
		[flagMutex lock];
		resetFlag = true;
		[flagMutex unlock];
		
		// launch the thread
		if([player isPlaying])
			[NSThread detachNewThreadSelector:@selector(fadeOut:)
									 toTarget:fader
								   withObject:nil];
		else
			[NSThread detachNewThreadSelector:@selector(switchTracks:)
									 toTarget:fader
								   withObject:nil];
	}
}

void iphoneMusicMgr_Deinit()
{
	[fader release];
	if(player)
		[player release];
}


unsigned int iphoneMusicMgr_GetCurPos()
{

	if(player) {
		[playerMutex lock];
		unsigned int t = [player currentTime];
		[playerMutex unlock];
		return t;
	}
	else
		return 0;
}

void iphoneMusicMgr_SetCurPos(unsigned int pos)
{
	posToSet = pos;
}

