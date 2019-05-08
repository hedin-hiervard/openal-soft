#import "CoreSurface.h"
//#import <CoreSurface/CoreSurface.h>
#import "ScreenView.h"
#import "QuartzCore/QuartzCore.h"

ScreenView *V = NULL;
CGRect r;

unsigned short* iPhone_getSurface() {
	CoreSurfaceBufferLock(screenSurface, 3);
	return CoreSurfaceBufferGetBaseAddress(screenSurface);
}
void iPhone_updateScreen() {
	CoreSurfaceBufferUnlock(screenSurface);
	[screenLayer setNeedsDisplay];
}

@implementation ScreenView 
- (id)initWithFrame:(CGRect)frame {
	if (self = [super initWithFrame:frame]) {
		V = self;
    }
	return self;
}

- (void)dealloc {
        [ screenLayer release ];
        pthread_mutex_destroy(&screenUpdateMutex);
        pthread_cond_destroy(&screenUpdateLock);
	[super dealloc];
}

bool init = false;

- (void)drawRect:(CGRect)rect{
	if(!init) {
		r = rect;
	int screenOrientation = 3;//[UIHardware deviceOrientation: YES];
	
	CFMutableDictionaryRef dict;
	int w = 768;
	int h = 1024;
	
	if (screenOrientation == 3) {
		w = 768;
		h = 1024;
	}
	
	int pitch = w * 2, allocSize = 2 * w * h;
	char *pixelFormat = "565L";
	
	//pthread_cond_init(&screenUpdateLock, NULL);
	//pthread_mutex_init(&screenUpdateMutex, NULL);
	
	dict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
									 &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFDictionarySetValue(dict, kCoreSurfaceBufferGlobal, kCFBooleanTrue);
	CFDictionarySetValue(dict, kCoreSurfaceBufferMemoryRegion,
						 CFSTR("PurpleGFXMem"));
	CFDictionarySetValue(dict, kCoreSurfaceBufferPitch,
						 CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &pitch));
	CFDictionarySetValue(dict, kCoreSurfaceBufferWidth,
						 CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &w));
	CFDictionarySetValue(dict, kCoreSurfaceBufferHeight,
						 CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &h));
	CFDictionarySetValue(dict, kCoreSurfaceBufferPixelFormat,
						 CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, pixelFormat));
	CFDictionarySetValue(dict, kCoreSurfaceBufferAllocSize,
						 CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &allocSize));
	
	screenSurface = CoreSurfaceBufferCreate(dict);
	CoreSurfaceBufferLock(screenSurface, 3);
		screenLayer =  self.layer;//[[CALayer layer] retain];
	[screenLayer setFrame: rect];
	[screenLayer setContents: screenSurface];
	[screenLayer setOpaque: YES];
	[screenLayer setHidden: NO];
//	[screenLayer setDelegate: self];
//	[self setLayer:screenLayer];
	
	//[[self _layer] addSublayer: screenLayer];
	//	self.layer = screenLayer;
	//[self addSublayer: screenLayer];
		//self.backgroundColor = [UIColor clearColor];
		
	CoreSurfaceBufferUnlock(screenSurface);
	init = true;
	} else {
		//char *x = NULL;
		//*x = 't';
	}
}

-(void)timerFired:(NSTimer *) timer
{		
	/*if(screenSurface) {
	unsigned short *addr = CoreSurfaceBufferGetBaseAddress(screenSurface);
	for(int i = 0; i < 320*480*2; i++)
		addr[i] = rand() % 255;
	}*/
//	[sharedInstance performSelectorOnMainThread:@selector(setNeedsDisplay) withObject:nil waitUntilDone: NO];
	//[self setNeedsDisplay];
	[screenLayer display];
	
}

@end
