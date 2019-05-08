#import <UIKit/UIKit.h>
#import "iPhoneAppDelegate.h"
#import "iPhoneViewController.h"
#import "iPhoneView.h"
#import "SplashViewController.h"
#import <Fabric/Fabric.h>
#import <Crashlytics/Crashlytics.h>

#import "GoogleConversionPing.h"

#import <StoreKit/StoreKit.h>

#include "iosdk/iosdk.h"

using namespace iosdk;

int pheroes_main();
int preload_resources();

NSLock *applicationLock;
bool bEligibleForOpenGL;

@implementation PHAppDelegate

@synthesize window;

-(id) init {
	self = [super init];
    return self;
}

- (void) mainLoop: (id)param
{
	pheroes_main();
	exit(0);
}


bool preinited = false;

- (void) preinit
{
    if(preinited)
        return;
    
    preload_resources();
    preinited = true;
}

- (BOOL)application:(UIApplication *)application willFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [self preinit];
    return YES;
}

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    
}


- (void)applicationDidFinishLaunching:(UIApplication *)application {
    [Fabric with:@[[Crashlytics class]]];

    Analytics::sharedInstance()->Init("UA-43821789-27");
    
    
    [self preinit];
    Analytics::sharedInstance()->TrackEvent("game", "appstart");
    
    applicationLock = [[NSLock alloc] init];
    bEligibleForOpenGL = true;
    
    CGRect  rect = [[UIScreen mainScreen] bounds];
    
	self.window = [[UIWindow alloc] initWithFrame:rect];
    
	iPhoneViewController* vc = [[iPhoneViewController alloc] init];
    
	[self.window setRootViewController: vc];
	[self.window makeKeyAndVisible];
    
    
	[NSThread detachNewThreadSelector:@selector(mainLoop:) toTarget:self withObject:nil];
    
    [(iPhoneView*)self.window.rootViewController.view updateFrame];
    
#if PLAYCITY //&& !defined(IPHONE_DEBUG)
    _splash = 0;
    [vc.view setHidden: true];
    [self performSelector: @selector(showSplash) withObject: nil afterDelay: 0.1f];
#else
    [self performSelector: @selector(splashEnded) withObject: nil afterDelay: 1.0f];
#endif
}

- (void) splashEnded
{

}

- (void) showSplash
{
    SplashViewController* splashCont = [[SplashViewController alloc] initWithSplashName:
                                        _splash == 0 ? @"playcity" : @"iosoftware"];
    
    splashCont.modalTransitionStyle = UIModalTransitionStyleCrossDissolve;
    [self.window.rootViewController presentModalViewController: splashCont animated: true];
    
    [self performSelector: @selector(hideSplash) withObject: nil afterDelay: 3.0f];
}

- (void) hideSplash
{
    [self.window.rootViewController dismissViewControllerAnimated: true completion: ^()
     {
         [(iPhoneView*)self.window.rootViewController.view updateFrame];
         _splash++;
         if(_splash < 2)
         {
             [self performSelector: @selector(showSplash) withObject: nil afterDelay: 0.1f];
         } else
         {
             [self.window.rootViewController.view setHidden: false];
             [self performSelector: @selector(splashEnded) withObject: nil afterDelay:0.1f];
         }
         
     }];
}



void onQuit();
void onSuspend();
void onResume();

- (void) applicationWillTerminate:(UIApplication *)application
{
	onQuit();
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    onSuspend();
    [applicationLock lock];
    bEligibleForOpenGL = false;
    [applicationLock unlock];
}
- (void)applicationWillEnterForeground:(UIApplication *)application
{
    onResume();
    [applicationLock lock];
    bEligibleForOpenGL = true;
    [applicationLock unlock];
}


- (void)applicationWillSuspend {
	onSuspend();
}

- (void)applicationResume:(struct __GSEvent *)event {
	onResume();
}

@end
