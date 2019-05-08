#ifndef GXLIB_GAPI_HELP_H_
#define GXLIB_GAPI_HELP_H_
#include <windows.h>
 
#define GETRAWFRAMEBUFFER   0x00020001
 
/*#define SHFS_SHOWTASKBAR            0x0001
#define SHFS_HIDETASKBAR            0x0002
#define SHFS_SHOWSIPBUTTON          0x0004
#define SHFS_HIDESIPBUTTON          0x0008
#define SHFS_SHOWSTARTICON          0x0010
#define SHFS_HIDESTARTICON          0x0020
 */
/*
typedef struct _RawFrameBufferInfo
{
   WORD wFormat;
   WORD wBPP;
   VOID *pFramePointer;
   int  cxStride;
   int  cyStride;
   int  cxPixels;
   int  cyPixels;
} RawFrameBufferInfo;
*/ 
                                                                                
#define false 0
#define true 1
 
typedef
struct GXDisplayProperties {
    DWORD cxWidth;
    DWORD cyHeight;         // notice lack of 'th' in the word height.
    long cbxPitch;          // number of bytes to move right one x pixel- can be negative.
    long cbyPitch;          // number of bytes to move down one y pixel- can be negative.
    long cBPP;              // # of bits in each pixel
    DWORD ffFormat;         // format flags.
}GXDisplayProperties;
 
 struct GXKeyList {
     short vkUp;             // key for up
     POINT ptUp;             // x,y position of key/button.  Not on screen but in screen coordinates.
     short vkDown;
     POINT ptDown;
     short vkLeft;
     POINT ptLeft;
     short vkRight;
     POINT ptRight;
     short vkA;
     POINT ptA;
     short vkB;
     POINT ptB;
     short vkC;
     POINT ptC;
     short vkStart;
     POINT ptStart;
 };
 
 #define kfLandscape 0x8         // Screen is rotated 270 degrees
 #define kfPalette 0x10  // Pixel values are indexes into a palette
 #define kfDirect 0x20  // Pixel values contain actual level information
 #define kfDirect555 0x40  // 5 bits each for red, green and blue valuesin a pixel.
 #define kfDirect565 0x80  // 5 red bits, 6 green bits and 5 blue bits per pixel
 #define kfDirect888 0x100  // 8 bits each for red, green and bluevalues in a pixel.
 #define kfDirect444 0x200  // 4 red, 4 green, 4 blue
 #define kfDirectInverted 0x400
 
 #define GX_FULLSCREEN 0x01  // for OpenDisplay()
 
 typedef GXDisplayProperties (*tGXDisplayProperties)(void);
 typedef int (*tGXOpenDisplay)(HWND, unsigned long);
 typedef void* (*tGXBeginDraw)(void);
 typedef int (*tGXEndDraw)(void);
 typedef int (*tGXCloseDisplay)(void);
 typedef int (*tGXSuspend)(void);
 typedef int (*tGXResume)(void);
 typedef int (*tGXCloseInput)(void);
 typedef int (*tGXOpenInput)(void);
  
 struct GAPI_funcs {
  tGXDisplayProperties dynamicGXGetDisplayProperties;
  tGXOpenDisplay dynamicGXOpenDisplay;
  tGXCloseDisplay dynamicGXCloseDisplay;
  tGXBeginDraw dynamicGXBeginDraw;
  tGXEndDraw dynamicGXEndDraw;
  tGXSuspend dynamicGXSuspend;
  tGXResume dynamicGXResume;
 };
 struct GAPI_properties {
  unsigned char invert;
  int colorscale;
  int dstPixelstep;
  int dstLinestep;
  int startOffset;
  int videoMode;
 };
// for importing entries from dll's..
#define IMPORT(Handle,Variable,Type,Function, Store) \
        printf( "get %s\n",Function ); \
         Variable = GetProcAddress(Handle, TEXT(Function)); \
         printf("store %s\n", Function ); \
         if (!Variable) { \
                 FreeLibrary(Handle); \
                 return NULL; \
         } \
         if (Store) \
                 Store = (Type)Variable;

// Although these flags can be unrelated they still
// have unique values.

#define GX_FULLSCREEN   0x01        // for OpenDisplay()
#define GX_NORMALKEYS   0x02
#define GX_LANDSCAPEKEYS    0
#endif //GXLIB_GAPI_HELP_H_
