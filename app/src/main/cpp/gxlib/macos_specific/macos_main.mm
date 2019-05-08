
#import <Cocoa/Cocoa.h>
#include "Foundation/NSThread.h"

#include "macos_video.h"

#import <pthread.h>

pthread_mutex_t _windowMutex;

void pheroes_main();
void preload_resources();

@interface MacOSWindow : NSWindow{
	
    bool bfullScreen;
}

- (bool) isFullScreen;

@end

@implementation MacOSWindow

- (id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)windowStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)deferCreation{

    bfullScreen = true;
    
    return [super initWithContentRect:contentRect styleMask:windowStyle backing:bufferingType defer:deferCreation];
}

- (bool) isFullScreen{

    return bfullScreen;
}

//Custom windows that use the NSBorderlessWindowMask can't become key by default. Override this method so that controls in this window will be enabled.

- (BOOL)canBecomeKeyWindow {
	
    return YES;	
}

- (void)keyDown:(NSEvent *)event{
}

- (void)keyUp:(NSEvent *)event{
}

- (void)toggleFullScreen:(id)sender{

    if( bfullScreen ){
    
        pthread_mutex_lock(&_windowMutex);
		[self setStyleMask:NSResizableWindowMask|NSMiniaturizableWindowMask|NSClosableWindowMask|NSTitledWindowMask];
        [self setTitle:@"PK2: Remastered"];
        pthread_mutex_unlock(&_windowMutex);
        
        bfullScreen = false;
    }
    else
        bfullScreen = true;
    
    [super toggleFullScreen:sender];
}

@end



@interface PalmHeroes2AppDelegate : NSObject <NSWindowDelegate, NSApplicationDelegate> {
    MacOSWindow *_window;
	MacOSView * view;
}

- (void) runMacMain:(id)inObject;
- (void) FullScreenMode;

@end

int main(int argc, char *argv[])
{
    return NSApplicationMain(argc,  (const char **) argv);
}

@implementation PalmHeroes2AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
#ifdef SHOP_PRODUCTION    
    if([[NSBundle mainBundle] respondsToSelector: @selector(appStoreReceiptURL)]) {
        // Insert code here to initialize your application 
        if(![[NSFileManager defaultManager] fileExistsAtPath:[[[NSBundle mainBundle] appStoreReceiptURL] path]]) {
            NSLog(@"to get sandbox receipt, the app must be launched from outside xcode");
            exit(173);
            
        }
    }
#endif
	
    NSRect fullscreenRect = [[NSScreen mainScreen] frame];
    
    CGFloat x, y;
    
    x = (fullscreenRect.size.width - 1024)/2;
    y = (fullscreenRect.size.height - 768)/2;
    
	//Create a screen-sized window on the display you want to take over:
	NSRect mainDisplayRect = NSMakeRect(x, y, 1024, 768);
	_window = [[MacOSWindow alloc] initWithContentRect: mainDisplayRect styleMask:NSBorderlessWindowMask backing:NSBackingStoreBuffered defer:YES];
	//Perform any other window configuration you desire:	
	[_window setOpaque:YES];
	
	[_window setHasShadow:false];
	
    [_window setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary];

    //Sets the minimum size of the window’s content view
    NSSize size = NSMakeSize(1024.0, 768.0);//788.0);
    [_window setContentMinSize:size];
    
	//Create a view with an double-buffered OpenGL context and attach it to the window:	
	NSOpenGLPixelFormatAttribute attrs[] = { NSOpenGLPFADoubleBuffer, 0 };	
	NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];	
	NSRect viewRect = NSMakeRect(0.0, 0.0, mainDisplayRect.size.width, mainDisplayRect.size.height); // NSMakeRect(100.0, 100.0, 1024.0, 788.0);
	view = [[MacOSView alloc] initWithFrame:viewRect pixelFormat: pixelFormat];	
	[_window setContentView: view];
	//Show the window:	
	[_window makeKeyAndOrderFront:self];
	
    [_window setDelegate:self];
    [_window toggleFullScreen:nil];
    
    [NSThread detachNewThreadSelector:@selector(runMacMain:) toTarget:self withObject:nil];
}

- (NSApplicationPresentationOptions)window:(NSWindow *)window willUseFullScreenPresentationOptions:(NSApplicationPresentationOptions)proposedOptions
{
    // customize our appearance when entering full screen:
    // we don't want the dock to appear but we want the menubar to hide/show automatically
    return (NSApplicationPresentationFullScreen |       // support full screen for this window (required)
            NSApplicationPresentationHideDock |         // completely hide the dock
            NSApplicationPresentationAutoHideMenuBar);  // yes we want the menu bar to show/hide
}

- (NSSize)window:(NSWindow *)window willUseFullScreenContentSize:(NSSize)proposedSize{
   
    // Return the result.
    NSRect mainDisplayRect = [[NSScreen mainScreen] frame]; 
    return mainDisplayRect.size;
}

- (NSArray *)customWindowsToEnterFullScreenForWindow:(NSWindow *)window{

    return nil;
}

- (void)window:(NSWindow *)window startCustomAnimationToEnterFullScreenWithDuration:(NSTimeInterval)duration{
}

- (NSArray *)customWindowsToExitFullScreenForWindow:(NSWindow *)window{

    return nil;
}

- (void)window:(NSWindow *)window startCustomAnimationToExitFullScreenWithDuration:(NSTimeInterval)duration{
}

- (void)didExitFull:(NSNotification *)notif{
}

- (void)willEnterFull:(NSNotification *)notif{
}


//Quit cocoa app on window close
-(BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
    return YES;
}

- (void) FullScreenMode{

	[view onUpdateSize]; 
}

- (void)applicationWillFinishLaunching:(NSNotification *)notification{

	NSString *_view = @"View";
	NSString *_fullscreen = @"Fullscreen";
	
	NSMenu *menu = [NSApp mainMenu];
	NSMenuItem *menuitem = [menu itemWithTitle:_view];
	
	if( menuitem ){
		
		menu = [menuitem submenu];		
		menuitem = [menu itemWithTitle:_fullscreen];
		
		if( menuitem ){
			
			[menuitem setAction:@selector(FullScreenMode)];
			[menuitem setEnabled:YES];
		}
	}
}

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)anItem{

	return( YES );
}

void onResume();
void onSuspend();

- (void)applicationDidBecomeActive:(NSNotification *)aNotification
{
	onResume();
}

- (void)applicationDidHide:(NSNotification *)aNotification
{
	onSuspend();
}

- (void) runMacMain:(id)inObject {
	
	[[NSAutoreleasePool alloc] init];
    preload_resources();
	pheroes_main();
	exit(0);
}

- (void)applicationWillTerminate:(NSNotification *)notification{
	[view onQuit];
	//exit(0);
}

void PleerPause();
void PleerPlay();

- (void)applicationWillHide:(NSNotification *)notification{

	PleerPause();
}
- (void)applicationWillUnhide:(NSNotification *)notification{
 
	PleerPlay();
}

@end
