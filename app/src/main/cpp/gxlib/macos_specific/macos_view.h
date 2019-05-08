//
//  macos_view.h
//  PalmHeroes2
//
//  Created by Илья on 19.04.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <OpenGL/gl.h>

@interface MacOSView : NSOpenGLView
{
//	NSBitmapImageRep * nsBitmapImageRepObj;
}

//- (NSBitmapImageRep *)GetNSBitmapImageRepObj;
- (void) drawRect: (NSRect) bounds;
- (void)updateSurface;
//- (id)initWithFrame:(NSRect)frameRect;

@end

