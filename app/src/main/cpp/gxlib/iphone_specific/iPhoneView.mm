#include "iPhoneView.h"
#include "iphone_common.h"

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <pthread.h>

iPhoneView *sharedInstance = nil;
static int _width = 0;
static int _height = 0;
static CGRect _screenRect;
static char* _textureBuffer = 0;
static int _textureWidth = 0;
static int _textureHeight = 0;
NSLock* _lock = nil;

//#include "gxl.common.def.h"

typedef unsigned char MouseId;
typedef unsigned char MouseButtonId;

void onMouseDown(int x, int y, MouseId mID, MouseButtonId mbID);
void onMouseUp(int x, int y, MouseId mID, MouseButtonId mbID);
void onMouseMove(int x, int y, MouseId mID, MouseButtonId mbID);
void onQuit();

// static long lastTick = 0;
// static int frames = 0;



void * iPhone_getSurface() {
	return _textureBuffer;
}

void iPhone_updateScreenRect(unsigned short* screen, int x1, int y1, int x2, int y2) {
	[_lock lock];
	
	int y;
	for (y = y1; y < y2; ++y) {
		memcpy(&_textureBuffer[(y * _textureWidth + x1 )* 2], &screen[y * _width + x1], (x2 - x1) * 2);
	}

	[_lock unlock];	
}


void iPhone_initSurface(int width, int height) {
	_width = width;
	_height = height;
//    [sharedInstance initSurface];
	[sharedInstance performSelectorOnMainThread:@selector(initSurface) withObject:nil waitUntilDone: YES];
}

bool iPhone_fetchEvent(int *outEvent, float *outX, float *outY) {
	id event = [sharedInstance getEvent];
	if (event == nil) {
		return false;
	}

	id type = [event objectForKey:@"type"];

	if (type == nil) {
		printf("fetchEvent says: No type!\n");
		return false;
	}

	*outEvent = [type intValue];
	*outX = [[event objectForKey:@"x"] floatValue];
	*outY = [[event objectForKey:@"y"] floatValue];
	return true;
}

const char* iPhone_getDocumentsDir() {
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	return [documentsDirectory UTF8String];
}

bool getLocalMouseCoords(CGPoint *point) {
	if (point->x < _screenRect.origin.x || point->x >= _screenRect.origin.x + _screenRect.size.width ||
		point->y < _screenRect.origin.y || point->y >= _screenRect.origin.y + _screenRect.size.height) {
			return false;
	}

	point->x = (point->x - _screenRect.origin.x) / _screenRect.size.width;
	point->y = (point->y - _screenRect.origin.y) / _screenRect.size.height;

	return true;
}

uint getSizeNextPOT(uint size) {
	//return size;
    if ((size & (size - 1)) || !size) {
        int log = 0;
		
        while (size >>= 1)
            ++log;
		
        size = (2 << log);
    }
	
    return size;
}

@implementation iPhoneView

+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

- (id)initWithFrame:(struct CGRect)frame {
	self = [super initWithFrame: frame];
//        
	_fullWidth = frame.size.width;
	_fullHeight = frame.size.height;
	_screenLayer = nil;

	sharedInstance = self;
	
	_lock = [NSLock new];
	_keyboardView = nil;
	_context = nil;
	_screenTexture = 0;

	return self;
}


-(void) dealloc {
	[super dealloc];

	if (_keyboardView != nil) {
		[_keyboardView dealloc];
	}
	
	if (_screenTexture)
		free(_textureBuffer);
}

- (void *)ce {
	return _screenSurface;
}

extern pthread_mutex_t suspend_mutex;
bool IsActive();

extern NSLock *applicationLock;
extern bool bEligibleForOpenGL;


- (void)updateSurface {
    [applicationLock lock];
    if(!bEligibleForOpenGL)
    {
        [applicationLock unlock];
        return;
    }
    [applicationLock unlock];
    
	if(!IsActive())
        return;

	GLfloat vertices[] = {
		0.0, 0.0f + _widthOffset,
		static_cast<GLfloat>(_visibleWidth), 0.0f + _widthOffset,
		0.0f,  static_cast<GLfloat>(_visibleHeight - _widthOffset),
		static_cast<GLfloat>(_visibleWidth),  static_cast<GLfloat>(_visibleHeight - _widthOffset)
	};

	float texWidth = _visibleWidth / (float)_textureWidth;
	float texHeight = _visibleHeight / (float)_textureHeight;
    
	const GLfloat texCoords[] = {
		texWidth, 0.0f,
		0.0f, 0.0f,
		texWidth, texHeight,
		0.0f, texHeight
	};
	
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoords);

	//[_lock lock];
	// Unfortunately we have to update the whole texture every frame, since glTexSubImage2D is actually slower in all cases
	// due to the iPhone internals having to convert the whole texture back from its internal format when used.
	// In the future we could use several tiled textures instead.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _textureWidth, _textureHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, _textureBuffer);
	//[_lock unlock];

	//glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, _viewRenderbuffer);
	[_context presentRenderbuffer:GL_RENDERBUFFER_OES];
//    glFlush();
//	pthread_mutex_unlock(&suspend_mutex);
}

- (void) setFrame:(CGRect)fr
{
    _fullWidth = fr.size.width;
    _fullHeight = fr.size.height;
    
    CGRect scr = [[UIScreen mainScreen] bounds];
    
    [super setFrame: CGRectMake((scr.size.width - _fullWidth) / 2, (scr.size.height - _fullHeight) / 2, _fullWidth, _fullHeight)];
}

- (void) updateFrame
{
    CGRect frame = [[UIScreen mainScreen] bounds];
    [self setFrame: frame];
}

- (void)initSurface {
	_textureWidth = getSizeNextPOT(_width);
	_textureHeight = getSizeNextPOT(_height);

//    CGRect screenBounds = [[UIScreen mainScreen] bounds];
//    CGFloat screenScale = [[UIScreen mainScreen] scale];
//    CGSize screenSize = CGSizeMake(screenBounds.size.width * screenScale, screenBounds.size.height * screenScale);
//    
//    CGSize normScreenSize = CGSizeMake(MAX(screenSize.width, screenSize.height), MIN(screenSize.width, screenSize.height));
    

    float scaleFactor = [[UIScreen mainScreen] scale];
    
	if (_context == nil) {
//		orientation = UIDeviceOrientationLandscapeRight;
		CAEAGLLayer *eaglLayer = (CAEAGLLayer*) self.layer;
		
        [eaglLayer setBounds: CGRectMake(0, 0, _width, _height)];
        
		eaglLayer.opaque = YES;
		eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
										[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGB565, kEAGLDrawablePropertyColorFormat, nil];
        
		
        eaglLayer.contentsScale = scaleFactor;
        
		_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		if (!_context || [EAGLContext setCurrentContext:_context]) {
			glGenFramebuffersOES(1, &_viewFramebuffer);
			glGenRenderbuffersOES(1, &_viewRenderbuffer);
			
			glBindFramebufferOES(GL_FRAMEBUFFER_OES, _viewFramebuffer);
			glBindRenderbufferOES(GL_RENDERBUFFER_OES, _viewRenderbuffer);
            
			[_context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(id<EAGLDrawable>)self.layer];
			glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, _viewRenderbuffer);
			
			glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &_backingWidth);
			glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &_backingHeight);			
            
			if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
				NSLog(@"Failed to make complete framebuffer object %x.", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
				return;
			}
			                        
            
			glViewport(0, 0, _width * scaleFactor, _height * scaleFactor);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			
			glEnable(GL_TEXTURE_2D);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnableClientState(GL_VERTEX_ARRAY);			
		}
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	    
    glRotatef(180, 0, 0, 1);
    
	glOrthof(0, _width, 0, _height, 0, 1);
	
	if (_screenTexture > 0)
		glDeleteTextures(1, &_screenTexture);

	glGenTextures(1, &_screenTexture);
	glBindTexture(GL_TEXTURE_2D, _screenTexture);
    
	glEnable(GL_TEXTURE_2D);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST  );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	
	if (_textureBuffer)
		free(_textureBuffer);
	
	int textureSize = _textureWidth * _textureHeight * 2;
	_textureBuffer = (char*)malloc(textureSize);
	memset(_textureBuffer, 0, textureSize);
	
	glDisable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, _viewRenderbuffer);
	[_context presentRenderbuffer:GL_RENDERBUFFER_OES];		
	
	if (_keyboardView != nil) {
		[_keyboardView removeFromSuperview];
		[[_keyboardView inputView] removeFromSuperview];
	}
    
    
    _visibleHeight = _height;// * scaleFactor;
    _visibleWidth = _width;// * scaleFactor;
    
//    NSLog(@"%f, %f, %f, %f", self.frame.origin.x, self.frame.origin.y, self.frame.size.width, self.frame.size.height);
    
//   [self setContentScaleFactor: 2.0f];
    
    
    [self updateFrame];
    //    NSLog(@"%f, %f, %f, %f", self.frame.origin.x, self.frame.origin.y, self.frame.size.width, self.frame.size.height);

    
#if 1
    _fullWidth = _width;
    _fullHeight = _height;
    
//	if (orientation == UIDeviceOrientationLandscapeLeft || orientation ==  UIDeviceOrientationLandscapeRight)
//    {
//		_visibleHeight = _height;
//		_visibleWidth = _width;
//
//		float ratioDifference = ((float)_height / (float)_width) / ((float)_fullWidth / (float)_fullHeight);
//		int rectWidth, rectHeight;
//		if (ratioDifference < 1.0f) {
//			rectWidth = _fullWidth * ratioDifference;
//			rectHeight = _fullHeight;
//			_widthOffset = (_fullWidth - rectWidth) / 2;
//			_heightOffset = 0;
//		} else {
//			rectWidth = _fullWidth;
//			rectHeight = _fullHeight / ratioDifference;
//			_heightOffset = (_fullHeight - rectHeight) / 2;
//			_widthOffset = 0;
//		}
//
//		//printf("Rect: %i, %i, %i, %i\n", _widthOffset, _heightOffset, rectWidth, rectHeight);
//		_screenRect = CGRectMake(_widthOffset, _heightOffset, rectWidth, rectHeight);
//    }
#endif
}


- (id)getEvent {
	if (_events == nil || [_events count] == 0) {
		return nil;
	}

	id event = [_events objectAtIndex: 0];

	[_events removeObjectAtIndex: 0];

	return event;
}

- (void)addEvent:(NSDictionary*)event {

	if(_events == nil)
		_events = [[NSMutableArray alloc] init];

	[_events addObject: event];
}

unsigned int touchId(void* key);
void removeTouch(void* key);

- (void) touchesEnded:(NSSet*) touches withEvent:(UIEvent *) event {
	NSArray* arr = [touches allObjects];
	for(int x=0; x<[touches count]; x++) { 
		UITouch* touch = [arr objectAtIndex:x];
		CGPoint p =  [touch locationInView:self];
		onMouseUp(p.x, p.y, touchId(touch), 0);
		removeTouch(touch);
	}
}

- (void) touchesMoved:(NSSet*) touches withEvent:(UIEvent *) event {
	NSArray* arr = [touches allObjects];
	for(int x=0; x<[touches count]; x++) { 
		UITouch* touch =  [arr objectAtIndex:x];
		CGPoint p =  [touch locationInView:self];
		onMouseMove(p.x, p.y, touchId(touch), 0);
	}
}


- (void) touchesBegan:(NSSet*) touches withEvent:(UIEvent *) event {
	NSArray* arr = [touches allObjects];
	for(int x=0; x<[touches count]; x++) { 
		UITouch* touch =  [arr objectAtIndex:x];
		CGPoint p =  [touch locationInView:self];
		onMouseDown(p.x, p.y, touchId(touch), 0);
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	NSArray* arr = [touches allObjects];
	for(int x=0; x<[touches count]; x++) { 
		UITouch* touch = [arr objectAtIndex:x];
		CGPoint p =  [touch locationInView:self];
		onMouseUp(p.x, p.y, touchId(touch), 0);
		removeTouch(touch);
	}	
}

- (void) onQuit
{
	onQuit();
}

- (void)handleKeyPress:(unichar)c {
	[self addEvent:
		[[NSDictionary alloc] initWithObjectsAndKeys:
		 [NSNumber numberWithInt:kInputKeyPressed], @"type",
		 [NSNumber numberWithFloat:(float)c], @"x",
		 [NSNumber numberWithFloat:0], @"y",
		 nil
		]
	];
}

- (BOOL)canHandleSwipes {
	return TRUE;
}

@end

