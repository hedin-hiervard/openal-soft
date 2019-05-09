void *frameBuffer;
//extern UIView *MAINVIEW;

// #import "CoreSurface.h"
// //#import <CoreSurface/CoreSurface.h>
// #import "ScreenView.h"

//#import "iphone_video_cs.h"
// extern CoreSurfaceBufferRef screenSurface;

unsigned short* Android_getSurface();

extern "C" { void Android_updateScreen();}

//extern iPhoneView *sharedInstance;
void *display_iphone_begin_draw()
{
	return Android_getSurface();
}

//void updateScreen();

extern ScreenView *V;

void display_android_end_draw()
{
	//[outLock unlock];
	//if(MAINVIEW)
	//	[MAINVIEW setNeedsDisplay];
	//[MAINVIEW timerFired];
	//updateScreen();
	//[V performSelectorOnMainThread:@selector(setNeedsDisplay) withObject:nil waitUntilDone: NO];
	//[sharedInstance updateScreen];
}
