//
//  AVAudioPlayerCategory.m
//  Enigma
//
//  Created by Pete Goodliffe on 05/04/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "AVAudioPlayer+PGFade.h"
#import <objc/runtime.h>

//==============================================================================
// Macro shenanigans to define methods that simulate properities using
// associative references

#define ASSOCIATIVE_PROPERTY_BASIC(NAME, SET_NAME, TYPE, NSNUMBER_MAKE, NSNUMBER_GET) \
    static char NAME##Key; \
    - (TYPE) NAME \
    { \
        NSNumber *number = (NSNumber *)objc_getAssociatedObject(self, & NAME##Key); \
        return number ? number.NSNUMBER_GET : (TYPE)0;\
    }\
    \
    - (void) SET_NAME:(TYPE)value\
    {\
        objc_setAssociatedObject(self, & NAME##Key, [NSNumber NSNUMBER_MAKE:value], OBJC_ASSOCIATION_RETAIN_NONATOMIC);\
    }

#define ASSOCIATIVE_PROPERTY_OBJ_COPY(NAME, SET_NAME, TYPE) \
    static char NAME##Key; \
    - (TYPE) fadeCompletion { return (TYPE)objc_getAssociatedObject(self, & NAME##Key); } \
    - (void) setFadeCompletion:(TYPE)value { objc_setAssociatedObject(self, & NAME##Key, value, OBJC_ASSOCIATION_COPY_NONATOMIC); }

//==============================================================================

static const NSTimeInterval fadeInterval    = 0.05;
static const float          floatNearEnough = 0.1;

static char fadeVolumeDeltaKey, fadeCompletionKey;

@implementation AVAudioPlayer (PGFade)

#pragma mark Properties

ASSOCIATIVE_PROPERTY_BASIC(fading,                  setFading,                  BOOL,  numberWithBool,  boolValue)
ASSOCIATIVE_PROPERTY_BASIC(playStopOriginalVolume,  setPlayStopOriginalVolume,  float, numberWithFloat, floatValue)
ASSOCIATIVE_PROPERTY_BASIC(fadeTargetVolume,        setFadeTargetVolume,        float, numberWithFloat, floatValue)
ASSOCIATIVE_PROPERTY_BASIC(fadeTargetPos,           setFadeTargetPos,           unsigned int, numberWithUnsignedInt, unsignedIntValue)
ASSOCIATIVE_PROPERTY_BASIC(fadeVolumeDelta,         setFadeVolumeDelta,         float, numberWithFloat, floatValue)
ASSOCIATIVE_PROPERTY_OBJ_COPY(fadeCompletion,       setFadeCompletion,          AVAudioPlayerFadeCompleteBlock)

#pragma mark Fading mechanics

-(void) callFadeWithDelay: (id) delay
{
    NSNumber *n = delay;
    [self performSelector:@selector(fadeFunction) withObject:nil afterDelay: [n floatValue]];  
}

- (void) fadeFunction
{
    if (!self.fading) return;

    const float target        = self.fadeTargetVolume;
    const float current       = self.volume;
    const float delta         = target-current;
    const float changePerStep = self.fadeVolumeDelta;



    if (fabs(delta) > fabs(changePerStep))
    {
        self.volume = current+changePerStep;
        [self performSelectorOnMainThread: @selector(callFadeWithDelay:) withObject: [NSNumber numberWithFloat: fadeInterval] waitUntilDone: false];
      //  NSLog(@"fading");
    }
    else
    {
       // NSLog(@"finished fading");
        self.volume = target;
        self.fading = NO;
        AVAudioPlayerFadeCompleteBlock completion = self.fadeCompletion;
        if (completion) completion();
        self.fadeCompletion = nil;
    }
}

- (void) fadeToVolume:(float)targetVolume duration:(NSTimeInterval)duration setPos: (bool)setPos
{
    [self fadeToVolume:targetVolume duration:duration setPos:setPos completion:nil];
}

- (void) fadeToVolume:(float)targetVolume duration:(NSTimeInterval)duration setPos: (bool)setPos completion:(AVAudioPlayerFadeCompleteBlock)completion
{
    if(setPos)
        self.currentTime = self.fadeTargetPos;
    if (duration <= 0 || fabs(targetVolume-self.volume) < floatNearEnough)
    {
        // Volume change is close enough, just go there immediately
        self.volume = targetVolume;
        if (completion) completion();
        return;
    }

    self.fading             = YES;
    self.fadeTargetVolume   = targetVolume;
    self.fadeVolumeDelta    = (targetVolume-self.volume)/(duration/fadeInterval);
    self.fadeCompletion     = completion;

    if(!self.isPlaying)
        [self play];
    [self fadeFunction];
}

- (void) stopWithFadeDuration:(NSTimeInterval)duration
{
    if (self.playing)
    {
        if (!self.fading) self.playStopOriginalVolume = self.volume;
        __block const float currentVolume = self.playStopOriginalVolume;

        [self fadeToVolume:0 duration:duration setPos:false completion:^{
            [self stop];
            self.volume = currentVolume;
        }];
    }
}

- (void) playWithFadeDuration:(NSTimeInterval)duration
{
    if (!self.fading) self.playStopOriginalVolume = self.volume;
    if (!self.playing) self.volume = 0;
    [self fadeToVolume:self.playStopOriginalVolume duration:duration setPos: false ];
}


@end
