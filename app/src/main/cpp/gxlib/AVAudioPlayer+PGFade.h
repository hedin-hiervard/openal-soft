//
//  AVAudioPlayer+PGFade.h
//  Enigma
//
//  Created by Pete Goodliffe on 05/04/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>

typedef void (^AVAudioPlayerFadeCompleteBlock)();


@interface AVAudioPlayer (PGFade)

@property (nonatomic,readonly) BOOL  fading;
@property (nonatomic,readonly) float fadeTargetVolume;
@property (nonatomic,readonly) unsigned int fadeTargetPos;

- (void) fadeToVolume:(float)volume duration:(NSTimeInterval)duration setPos: (bool)setPos;
- (void) fadeToVolume:(float)targetVolume duration:(NSTimeInterval)duration setPos: (bool)setPos completion:(AVAudioPlayerFadeCompleteBlock)completion;

- (void) stopWithFadeDuration:(NSTimeInterval)duration;
- (void) playWithFadeDuration:(NSTimeInterval)duration;

@end
