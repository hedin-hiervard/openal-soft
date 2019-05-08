//
//  macos_view.m
//  PalmHeroes2
//
//  Created by Илья on 19.04.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "macos_view.h"
#include "iphone_common.h"

static MacOSView* sharedInstance = nil;
static int _needsScreenUpdate = 0;

//static int _width = 0;
//static int _height = 0;

GLint _width = 0;
GLint _height = 0;

u_int16_t* pixels = nil;

//NSLock* _lock = nil;

@implementation MacOSView


-(void) drawRect: (NSRect) bounds

{

	if (!_needsScreenUpdate) {
		return;
	}
	
	_needsScreenUpdate = 0;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	glDrawPixels(_width, _height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pixels);
	glFlush ();
	
//	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}


- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format{
	
	self = [super initWithFrame:frameRect pixelFormat:format];
	
	if (self != nil) {
		
		sharedInstance = self;
		
		_width = frameRect.size.width;
		_height = frameRect.size.height;

		//_lock = [NSLock new];
		
		
//		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}
	
	return self;
	
}

- (void)updateSurface{

//	if (!_needsScreenUpdate) {
//		return;
//	}
//	_needsScreenUpdate = 0;

//	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
/*	
	if( [self lockFocusIfCanDraw] ){
	
	glClear(GL_COLOR_BUFFER_BIT);
	
//	glRasterPos2i(0,0);
	glDrawPixels(_width, _height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pixels);
	glFlush ();
	
		[self unlockFocus];

}
*/	

//	gluPerspective(60.0, 1.0, 0.1, 1000.0);
//	glMatrixMode(GL_MODELVIEW);
//	glDisable(GL_DITHER);
	
	
	
//	glClear(GL_COLOR_BUFFER_BIT);
	
//	[_lock lock];
//	glDrawPixels(_width, _height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pixels);
//	[_lock unlock];
//	glFlush ();
}

- (void) mouseDown: (NSEvent *) theEvent
{
	NSPoint pt = [theEvent locationInWindow];
}


@end

void MacPaint( u_int16_t *_pixels, int _x, int _y ){
	
	pixels = _pixels;
	_width = _x;
	_height = _y;
		
//	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

//	glViewport(0,0,_width,_height);
/*	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,_width,0,_height,-1.0,1.0);
	glMatrixMode(GL_MODELVIEW);
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	glRasterPos2i(0,0);
	glDrawPixels(_width, _height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pixels);
	glFlush ();
*/

	if (!_needsScreenUpdate) {
		_needsScreenUpdate = 1;

	[sharedInstance performSelectorOnMainThread:@selector(drawRect:) withObject:nil waitUntilDone: NO];
		
//		[sharedInstance updateSurface];
	}

	//u_int16_t* pixels = _pixels;
	
	//	_DrowArray( _pixels, _x, _y );
	
	//	glClear ( GL_COLOR_BUFFER_BIT );
	
	
	
	//	glFlush ();
}
