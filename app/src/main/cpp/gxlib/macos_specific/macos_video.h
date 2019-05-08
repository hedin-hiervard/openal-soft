
#import <Cocoa/Cocoa.h>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

@interface MacOSView : NSOpenGLView {

	bool bFullscreen;
    bool flag;
}

- (void) _init;
- (void) _updateScreen:(NSRect)_rect;
- (void) onQuit;

- (void) onUpdateSize;
- (bool) isFullScreen;
- (void) onResize;
- (bool) isFlag;
- (void) setFlag:(bool)_flag;

@end
