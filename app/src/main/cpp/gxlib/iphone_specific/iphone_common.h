#ifndef GXLIB_IPHONE_SPECIFIC_IPHONE_COMMON_H_
#define GXLIB_IPHONE_SPECIFIC_IPHONE_COMMON_H_
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


FILE *fopen_iphone(const char *, const char *);
const char * GetBundlePath();
const char * GetDocumentsPath();
const char * GetLibraryPath();

u_int32_t GetScreenWidth();
u_int32_t GetScreenHeight();
u_int32_t GetWindowWidth();
u_int32_t GetWindowHeight();
bool IsRetina();

// We need this to be able to call functions from/in Objective-C.
#ifdef  __cplusplus
extern "C" {
#endif
	
// On the C++ side
void iphone_main(int argc, char *argv[]);

#if defined( OS_IPHONE )
// On the ObjC side
void iPhone_updateScreenRect(unsigned short* screen, int x1, int y1, int x2, int y2);
void iPhone_initSurface(int width, int height);
bool iPhone_fetchEvent(int *outEvent, float *outX, float *outY);
const char* iPhone_getDocumentsDir();
#endif
	
#if defined( OS_MACOS )
void macos_updateScreen();
void* macos_getSurface();
void macos_initScreen( int _x, int _y );
int macos_getCurScreenWidth();
int macos_getCurScreenHeight();	
int macos_offset();
bool isFullScreen();
#endif

#ifdef  __cplusplus
}		
#endif
	

#endif //GXLIB_IPHONE_SPECIFIC_IPHONE_COMMON_H_
