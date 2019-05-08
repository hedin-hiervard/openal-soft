
#import "macos_video.h"
#include "iphone_common.h"

#include "Foundation/NSThread.h"
#include <AppKit/NSResponder.h>
#import <pthread.h>

pthread_mutex_t initMutex;
pthread_mutex_t backbufMutex;
pthread_mutex_t windowMutex;

static MacOSView* sharedInstance = nil;
static int _needsScreenUpdate = 0;
static int _width = 0;
static int _height = 0;
static char* _buffer = 0;
static int _textureWidth = 0;
static int _textureHeight = 0;

typedef unsigned char MouseId;
typedef unsigned char MouseButtonId;

void onMouseDown(int x, int y, MouseId mID, MouseButtonId mbID);
void onMouseUp(int x, int y, MouseId mID, MouseButtonId mbID);
void onMouseMove(int x, int y, MouseId mID, MouseButtonId mbID);
void onRightMouseDown(int x, int y, MouseId mID, MouseButtonId mbID);
void onRightMouseUp(int x, int y, MouseId mID, MouseButtonId mbID);
void onMouseEntered(int x, int y, MouseId mID);
void onMouseLeave(int x, int y, MouseId mID);
void onMouseWheelDown(sint16 line, MouseId mID);
void onMouseWheelUp(sint16 line, MouseId mID);
void onQuit();
void onResume();
void onResize();
void onSuspend();
bool IsInMenu();
bool ExitMenu();
bool ExitDlg();
bool Space();
bool Tab( bool bDown );
bool SetKeyUpFlag( bool flag );
bool SetKeyDownFlag( bool flag );
bool SetKeyLeftFlag( bool flag );
bool SetKeyRightFlag( bool flag );
bool OnWait();
bool QuickSave();
bool QuickLoad();
bool Esc();
bool Enter();
bool ShowSaveScreen();
bool ShowLoadScreen();
bool AutoCombat( bool bEnd );
void Num( unsigned short ch );
bool CastSpell();
bool EndTurn();
void Minimize();
bool MiniMap( bool bDown );
bool ExitDlgClick();
void SetToolbarInfo(int px, int py);
void onKeyDown( sint32 key );
void PleerPlay();
void PleerPause();
bool CenterMap();
bool CastleMarket();
bool CastleTavern();
bool CastleRecruit();
bool CastleBuild();
bool NextPrevCastle( bool bNext );
bool Defender();

void* macos_getSurface()
{
	return _buffer;
}

uint getSizeNextPOT(uint size) {
    if ((size & (size - 1)) || !size) {
        int log = 0;
		
        while (size >>= 1)
            ++log;
		
        size = (2 << log);
    }
	
    return size;
}

@implementation MacOSView

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format{
	
    pthread_mutex_init(&windowMutex, NULL);
    self = [super initWithFrame:frameRect pixelFormat:format];
    if (self) {
        // Initialization code here.
		
		[[self openGLContext] makeCurrentContext];
		
		// Setup some basic OpenGL stuff		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glEnable(GL_RASTER_POSITION_UNCLIPPED_IBM);
		glDisable(GL_DITHER);
		
		sharedInstance = self;
		_buffer = nil;
		_width = frameRect.size.width;
		_height = frameRect.size.height;
		
		bFullscreen = true;
        flag = false;
				
		[self setNeedsDisplay:true];
    }
    return self;
}
		
-(void) dealloc {
	[super dealloc];
	
	if (_buffer)
		free(_buffer);
}

- (void)drawRect:(NSRect)dirtyRect {
    // Drawing code here.
}

- (void) _init{    
    pthread_mutex_lock(&initMutex);

	_textureWidth = getSizeNextPOT(_width);
	_textureHeight = getSizeNextPOT(_height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glRotatef(180, 0, 0, 1);
	
    
	if (_buffer)
		free(_buffer);
	
	int _size = _textureWidth * _textureHeight * 4;
	_buffer = (char*)malloc(_size);
    memset(_buffer, 0, _size);
    
    pthread_mutex_unlock(&initMutex);
}

- (void) _updateScreen:(NSRect)_rect{

	if( _needsScreenUpdate == 0 ) {
		return;
    }
	
    pthread_mutex_lock(&initMutex);
    
	_needsScreenUpdate = 0;
	
	NSOpenGLContext* context = [self openGLContext];
	
    if ([context view] != self) {
		
        [context setView:self];		
    }
	
    [context makeCurrentContext];

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if( [sharedInstance lockFocusIfCanDraw] ){
		
		glDrawPixels(_width, _height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, _buffer);//pixels);
		[sharedInstance unlockFocus];
	
		glFinish();
	}
    pthread_mutex_unlock(&initMutex);
		
	[context flushBuffer];
}

- (void)setFrameSize:(NSSize)newSize{
/*
	if( newSize.width < 1024 )
		newSize.width = 1024;
	
	if( newSize.height < 768 )
		newSize.height = 768;
*/	
	[super setFrameSize:newSize];
	
//	onResize();
}

- (void)setFrame:(NSRect)frameRect{

	[super setFrame:frameRect];
}

- (void) update // window resizes, moves and display changes (resize, depth and display config change)
{
    NSRect rect = [self bounds];
    
    [super update];
	
    [[self openGLContext] makeCurrentContext];
    [[self openGLContext] update];
	
    glViewport(0, 0, (GLint) rect.size.width, (GLint) rect.size.height);    
	
    glMatrixMode(GL_PROJECTION);	
    glLoadIdentity();
	
    gluOrtho2D(-(rect.size.width/2), rect.size.width/2, -(rect.size.height/2), rect.size.height/2);    
	
    glMatrixMode(GL_MODELVIEW);	
    glLoadIdentity(); 
    
	onResize();
    
    [self setNeedsDisplay:true];
}

- (void) reshape{    
	
    NSRect rect = [self bounds];
    
    [super reshape];
	
	[[self openGLContext] makeCurrentContext];
    [[self openGLContext] update];
		
	glViewport(0, 0, (GLint) rect.size.width, (GLint) rect.size.height);    
	
    glMatrixMode(GL_PROJECTION);	
    glLoadIdentity();
	
    gluOrtho2D(-(rect.size.width/2), rect.size.width/2, -(rect.size.height/2), rect.size.height/2);    
	
    glMatrixMode(GL_MODELVIEW);	
    glLoadIdentity(); 
	
	onResize();
	
	[self setNeedsDisplay:true];
}

- (void) onUpdateSize{

    [[self window] toggleFullScreen:nil];
    
	NSRect mainDisplayRect, fullscreenRect;
	
	fullscreenRect = [[NSScreen mainScreen] frame];
	
	if( bFullscreen ){
		
        [NSApp setPresentationOptions:NSApplicationPresentationDefault];
        
		CGFloat x, y;
		
		x = (fullscreenRect.size.width - 1024)/2;
		y = (fullscreenRect.size.height - 768)/2;
		
		mainDisplayRect = NSMakeRect(x, y, 1024, 788);
		
        pthread_mutex_lock(&windowMutex);
		[[self window] setStyleMask:NSResizableWindowMask|NSMiniaturizableWindowMask|NSClosableWindowMask|NSTitledWindowMask];
        [[self window] setTitle:@"PK2: Remastered"];
		[[self window] setFrame: mainDisplayRect display:YES];
        pthread_mutex_unlock(&windowMutex);
		mainDisplayRect.origin.x += 20;
		mainDisplayRect.size.height -= 20;
        
		bFullscreen = false;
	}
	else {
        
         [NSApp setPresentationOptions:NSApplicationPresentationAutoHideMenuBar | NSApplicationPresentationHideDock];	
		
		mainDisplayRect = NSMakeRect( 0.0, 0.0, fullscreenRect.size.width, fullscreenRect.size.height );
		[[self window] setStyleMask:NSBorderlessWindowMask];
        pthread_mutex_lock(&windowMutex);
		[[self window] setFrame: mainDisplayRect display:YES];
        pthread_mutex_unlock(&windowMutex);
		
		bFullscreen = true;
	}
	
    pthread_mutex_lock(&windowMutex);

	[self setFrame:mainDisplayRect];

	[[self window] setContentView: self];
	[[self window] makeFirstResponder:self];
    
    pthread_mutex_unlock(&windowMutex);

	onResize();
}

- (void) onResize{
/*
    NSRect mainDisplayRect, fullscreenRect;
	
	fullscreenRect = [[NSScreen mainScreen] frame];
	
	if( bFullscreen ){
		
        //        [NSApp setPresentationOptions:NSApplicationPresentationDefault];
        
		CGFloat x, y;
		
		x = (fullscreenRect.size.width - 1024)/2;
		y = (fullscreenRect.size.height - 768)/2;
		
		mainDisplayRect = NSMakeRect(x, y, 1024, 788);
		
        pthread_mutex_lock(&windowMutex);
		[[self window] setStyleMask:NSResizableWindowMask|NSMiniaturizableWindowMask|NSClosableWindowMask|NSTitledWindowMask];
		[[self window] setFrame: mainDisplayRect display:YES];
        pthread_mutex_unlock(&windowMutex);
		mainDisplayRect.origin.x += 20;
		mainDisplayRect.size.height -= 20;
        
		bFullscreen = false;
	}
	else {
        
        //         [NSApp setPresentationOptions:NSApplicationPresentationHideMenuBar | NSApplicationPresentationHideDock];	
		
		mainDisplayRect = NSMakeRect( 0.0, 0.0, fullscreenRect.size.width, fullscreenRect.size.height );
		[[self window] setStyleMask:NSBorderlessWindowMask];
        pthread_mutex_lock(&windowMutex);
		[[self window] setFrame: mainDisplayRect display:YES];
        pthread_mutex_unlock(&windowMutex);
		
		bFullscreen = true;
	}
	
    pthread_mutex_lock(&windowMutex);
    
	[self setFrame:mainDisplayRect];
    
	[[self window] setContentView: self];
	[[self window] makeFirstResponder:self];
    
    pthread_mutex_unlock(&windowMutex);
    
    onResize();
*/
}

- (bool) isFlag{

    return flag;
}
- (void) setFlag:(bool)_flag{

    flag = _flag;
}

- (void) mouseDown: (NSEvent *) theEvent
{

    NSPoint mouse = [theEvent locationInWindow];
	
	onMouseDown( (int)mouse.x, (int)(_height - mouse.y), 0, 0 );
}

// so that we can get keyboard events
- (BOOL)acceptsFirstResponder {
	return YES;
}

- (void)keyDown:(NSEvent *)event {
 
	if (([event modifierFlags] & NSDeviceIndependentModifierFlagsMask) == NSCommandKeyMask){
		
		//if ([[event charactersIgnoringModifiers] isEqualToString:@"q"]){
		if( [event keyCode] == 12 ){ //q
			
			[self onQuit];
			return;
		}
		else if( [event keyCode] == 1 ){ //s
				
				if( QuickSave() )
					return;
		}
		else if( [event keyCode] == 37 ){ //l
			
				if( QuickLoad() )
					return;
		}
		else if( [event keyCode] == 4 ){ //h
			
				[NSApp hide:nil];
				return;
		}
		else if( [event keyCode] == 46 ){ //m
			//minimize
		}
		else if( [event keyCode] == 3 ){ //f
			
				//[self onUpdateSize];
				return;
		}
	}
	else if( ([event modifierFlags] & NSDeviceIndependentModifierFlagsMask) == NSCommandKeyMask+NSShiftKeyMask ){
			
			if( [event keyCode] == 1 ){ //S
			
				if( ShowSaveScreen() )
					return;
			}
			else if( [event keyCode] == 37 ){ //L
					
					if( ShowLoadScreen() )
						return;
			}
	}	
	//Wait
	else if( [event keyCode] == 13 ){ //w
			
			if( OnWait() )
				return;
	}
	//Autocombat 
	else if( [event keyCode] == 0 ){ //a
			
			if( AutoCombat(false) )
				return;
            if( NextPrevCastle( false ) )
                return;
	}
    else if( [event keyCode] == 2 ){ //d

        if( NextPrevCastle( true ) )
            return;
        if( Defender() )
            return;
	}
	else if( [event keyCode] == 8 ){ //c
		
            if( CenterMap() )
                return;
	}
    //CastSpell
    else if( [event keyCode] == 1 ){ //s
		
            if( CastSpell() )
                return;
	}
	else if( [event keyCode] == 14 ){ //e
			
			if( EndTurn() )
				return;
	}
	else if( [event keyCode] == 46 ){ //m
			
			if( MiniMap(true) )
				return;
	}
    else if( [event keyCode] == 11 ){ //B
        
        if( CastleBuild() )
            return;
	}
    else if( [event keyCode] == 15 ){ //R
        
        if( CastleRecruit() )
            return;
	}
    else if( [event keyCode] == 17 ){ //T
        
        if( CastleTavern() )
            return;
	}
	else if ([[event characters] characterAtIndex:0] == NSUpArrowFunctionKey) {
	
		if( SetKeyUpFlag(true) )
			return;
	}
	else if ([[event characters] characterAtIndex:0] == NSDownArrowFunctionKey) {
		
		if( SetKeyDownFlag(true) )
			return;
	}
	else if ([[event characters] characterAtIndex:0] == NSLeftArrowFunctionKey) {
		
		if( SetKeyLeftFlag(true) )
			return;
	}
	else if ([[event characters] characterAtIndex:0] == NSRightArrowFunctionKey) {
		
		if( SetKeyRightFlag(true) )
			return;
	}
	//Enter
	else if( [event keyCode] == 36 ){
	
		if( Enter() )
			return;
	}
	//Tab
	else if( [event keyCode] == 48 ){
		
		if( Tab(true) )
			return;
	}
	//Space
	else if( [event keyCode] == 49 ){
		
		if( Space() )
			return;
	}
	//Esc
	else if( [event keyCode] == 53 ){
		
		if( Esc() )
			return;
	}
	//End AutoCombat
	AutoCombat(true);
	
	//Num
	Num( [event keyCode] );
	
	ExitDlgClick();
	
	[super keyDown:event];
}

- (void)keyUp:(NSEvent *)event{

	if ([[event characters] characterAtIndex:0] == NSUpArrowFunctionKey) {
		
		SetKeyUpFlag(false);
		return;
	}
	if ([[event characters] characterAtIndex:0] == NSDownArrowFunctionKey) {
		
		SetKeyDownFlag(false);
		return;
	}
	if ([[event characters] characterAtIndex:0] == NSLeftArrowFunctionKey) {
		
		SetKeyLeftFlag(false);
		return;
	}
	if ([[event characters] characterAtIndex:0] == NSRightArrowFunctionKey) {
		
		SetKeyRightFlag(false);
		return;
	}
	//Tab
	if ([event keyCode] == 48/*Tab*/){
		
		Tab(false);
		return;
	}
	//if ([[event charactersIgnoringModifiers] isEqualToString:@"m"]){
	if( [event keyCode] == 46 ){ //m	
//		MiniMap(false);
		return;
	}
}

- (void)mouseUp:(NSEvent *)theEvent {

	NSPoint mouse = [theEvent locationInWindow];
	
	onMouseUp( (int)mouse.x, (int)(_height - mouse.y), 0, 0);//NSLeftMouseUp );
}

- (void)mouseMoved:(NSEvent *)theEvent{

	NSPoint mouse = [theEvent locationInWindow];
	
    //NSLog(@"Entered\n");
	onMouseEntered( (int)mouse.x, (int)(_height - mouse.y), 0 );
//	SetToolbarInfo((int)mouse.x, (int)(_height - mouse.y));
}

- (void)rightMouseDragged:(NSEvent *)theEvent{

    NSPoint mouse = [theEvent locationInWindow];

	onMouseEntered( (int)mouse.x, (int)(_height - mouse.y), 0 );
}

- (void)mouseDragged:(NSEvent *)theEvent{

	NSPoint mouse = [theEvent locationInWindow];
	
	onMouseMove( (int)mouse.x, (int)(_height - mouse.y), 0, 0);//NSLeftMouseUp );
}

- (void)rightMouseUp:(NSEvent *)theEvent{

    NSPoint mouse = [theEvent locationInWindow];
    onRightMouseUp((int)mouse.x, (int)(_height - mouse.y), 0, 0);
}

- (void)rightMouseDown:(NSEvent *)theEvent{

    NSPoint mouse = [theEvent locationInWindow];
	onRightMouseDown((int)mouse.x, (int)(_height - mouse.y), 0, 0);
}

- (void)mouseEntered:(NSEvent *)theEvent{

	NSPoint mouse = [theEvent locationInWindow];
//	onMouseEntered( (int)mouse.x, (int)(_height - mouse.y), 0 );
}

- (void)mouseExited:(NSEvent *)theEvent{
	
	NSPoint mouse = [theEvent locationInWindow];
//	onMouseLeave( (int)mouse.x, (int)(_height - mouse.y), 0 );
}

- (void)scrollWheel:(NSEvent *)theEvent{

	NSLog(@"%f\n", [theEvent deltaY] );
	
	if( [theEvent deltaY] < 0 )
		
		onMouseWheelDown( (sint16)([theEvent deltaY]-1.0), 0 );
	else if( [theEvent deltaY] > 0 )
		onMouseWheelDown( (sint16)([theEvent deltaY]+1.0), 0 );
}

- (void)viewDidMoveToWindow{

	[[self window] setAcceptsMouseMovedEvents:YES];
}

- (void) onQuit{
	
	onQuit();
}

- (bool) isFullScreen{

	return bFullscreen;
}

@end

void macos_updateScreen(){

	if (!_needsScreenUpdate) {
		_needsScreenUpdate = 1;
	
		[sharedInstance performSelectorOnMainThread:@selector(_updateScreen:) withObject:nil waitUntilDone: NO];
		
	}	
}

void macos_initScreen( int _x, int _y ){
	_width = _x;
	_height = _y;

	[sharedInstance performSelectorOnMainThread:@selector(_init) withObject:nil waitUntilDone: NO];	
}

int macos_getCurScreenWidth()
{
    if(pthread_mutex_trylock(&windowMutex) != 0)
        return 1024;
    
	NSRect rect = [[sharedInstance window] frame];
    pthread_mutex_unlock(&windowMutex);
	return( rect.size.width );
}

int macos_getCurScreenHeight()
{
    if(pthread_mutex_trylock(&windowMutex) != 0)
        return 768;
    
   NSRect rect = [[sharedInstance window] frame];
    pthread_mutex_unlock(&windowMutex);
	return( rect.size.height );	
}

int macos_offset(){

	if( ![[sharedInstance window] isFullScreen] )
		return 0;
	else 
		return 22;
}

bool isFullScreen(){

    return ![[sharedInstance window] isFullScreen];
}
