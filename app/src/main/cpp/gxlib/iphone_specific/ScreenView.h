#ifndef GXLIB_IPHONE_SPECIFIC_SCREENVIEW_H_
#define GXLIB_IPHONE_SPECIFIC_SCREENVIEW_H_
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "CoreSurface.h"
//#import <CoreSurface/CoreSurface.h>
#include <pthread.h>

CoreSurfaceBufferRef screenSurface;
CALayer *screenLayer;
pthread_cond_t screenUpdateLock;
pthread_mutex_t screenUpdateMutex;

@interface ScreenView : UIView 
{
		int initGraphics;
}

- (id)initWithFrame:(CGRect)frame;
- (void)dealloc;
- (void)drawRect:(CGRect)frame;
//- (void)updateScreen;
- (void)timerFired:(NSTimer *) timer;

@end
#endif //GXLIB_IPHONE_SPECIFIC_SCREENVIEW_H_
