#import <UIKit/UIKit.h>

CGDataProviderRef provider;
void *frameBuffer;
CGColorSpaceRef colorSpaceRef;
CGImageRef ir;
CGContextRef context;

NSLock *outLock;
//extern UIView *MAINVIEW;

#import "CoreSurface.h" 
//#import <CoreSurface/CoreSurface.h>
#import "ScreenView.h"

//#import "iphone_video_cs.h"
extern CoreSurfaceBufferRef screenSurface;

unsigned short* iPhone_getSurface();

extern "C" { void iPhone_updateScreen();}

//extern iPhoneView *sharedInstance;
void *display_iphone_begin_draw()
{
	return iPhone_getSurface();
}

//void updateScreen();

extern ScreenView *V;

void display_iphone_end_draw()
{
	//[outLock unlock];
	//if(MAINVIEW)
	//	[MAINVIEW setNeedsDisplay]; 
	//[MAINVIEW timerFired];
	//updateScreen();
	//[V performSelectorOnMainThread:@selector(setNeedsDisplay) withObject:nil waitUntilDone: NO];
	//[sharedInstance updateScreen];
}