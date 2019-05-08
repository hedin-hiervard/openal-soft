    
#import <UIKit/UIKit.h>
#import <Foundation/NSThread.h>
#import "iPhoneViewController.h"

#import "TapjoyConnect/TapjoyConnect.h"

extern "C" {
	void iPhone_initSurface(int width, int height);
}
void pheroes_main();
inline bool IsInMenu();
void SetLanguage(const char *str);

static int gArgc;
static char** gArgv;

int main(int argc, char *argv[]) {
	gArgc = argc;
	gArgv = argv;

    NSAutoreleasePool *autoreleasePool = [ 
        [ NSAutoreleasePool alloc ] init
    ];

    int returnCode = UIApplicationMain(argc, argv, @"PHAppDelegate", @"PHAppDelegate");
    [ autoreleasePool release ];
    return returnCode;
}


