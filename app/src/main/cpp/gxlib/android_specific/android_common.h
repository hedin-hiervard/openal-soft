#ifndef GXLIB_Android_SPECIFIC_Android_COMMON_H_
#define GXLIB_Android_SPECIFIC_Android_COMMON_H_

#include "stdafx.h"

enum InputEvent {
	kInputMouseDown,
	kInputMouseUp,
	kInputMouseDragged,
	kInputMouseSecondDragged,
	kInputMouseSecondDown,
	kInputMouseSecondUp,
	kInputOrientationChanged,
	kInputKeyPressed,
	kInputApplicationSuspended,
	kInputApplicationResumed,
	kInputSwipe
};

enum ScreenOrientation {
	kScreenOrientationPortrait,
	kScreenOrientationLandscape,
	kScreenOrientationFlippedLandscape
};

typedef enum
{
	kUIViewSwipeUp = 1,
	kUIViewSwipeDown = 2,
	kUIViewSwipeLeft = 4,
	kUIViewSwipeRight = 8
} UIViewSwipeDirection;


// FILE *fopen_Android(const char *, const char *);
const char * GetBundlePath();
const char * GetDocumentsPath();
const char * GetLibraryPath();

uint32 GetScreenWidth();
uint32 GetScreenHeight();
uint32 GetWindowWidth();
uint32 GetWindowHeight();
bool IsRetina();

// We need this to be able to call functions from/in Objective-C.
#ifdef  __cplusplus
extern "C" {
#endif

// On the C++ side
void Android_main(int argc, char *argv[]);

#if defined( OS_ANDROID )
// On the ObjC side
void Android_updateScreenRect(unsigned short* screen, int x1, int y1, int x2, int y2);
void Android_initSurface(int width, int height);
bool Android_fetchEvent(int *outEvent, float *outX, float *outY);
const char* Android_getDocumentsDir();
#endif

#ifdef  __cplusplus
}
#endif


#endif //GXLIB_Android_SPECIFIC_Android_COMMON_H_
