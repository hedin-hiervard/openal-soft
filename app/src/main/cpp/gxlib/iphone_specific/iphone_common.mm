#if defined(OS_IPHONE)
#import "UIKit/UIKit.h"
#endif
#if defined(OS_MACOS)
#import <Cocoa/Cocoa.h>
#include "iphone_common.h"
#endif
//#import "i"

void DebugOutput(const char * str) {
	NSString* s = [NSString  stringWithCString: str];
	NSLog(s);
}

const char * GetBundlePath() {
	NSString * str = [[NSBundle mainBundle] bundlePath];
	return [str cStringUsingEncoding:NSASCIIStringEncoding];
}

const char * GetLibraryPath() {
#ifdef OS_MACOS
    assert(false);
    return NULL;
#else
	NSArray *paths = NSSearchPathForDirectoriesInDomains(
#if defined(OS_IPHONE)
                                                         NSLibraryDirectory
#endif // for some reason, #elseif doesn't work here
                                                         , NSUserDomainMask, YES); 
    
	NSString *str = [paths objectAtIndex:0];
	return [str cStringUsingEncoding:NSASCIIStringEncoding];
#endif
}


const char * GetDocumentsPath() {
	NSArray *paths = NSSearchPathForDirectoriesInDomains(
#if defined(OS_IPHONE)
		NSDocumentDirectory
#endif // for some reason, #elseif doesn't work here
#if defined(OS_MACOS)
		NSApplicationSupportDirectory
#endif
	, NSUserDomainMask, YES); 
		
	NSString *str = [paths objectAtIndex:0];
#ifdef OS_MACOS
	str = [str stringByAppendingString: @"/Palm Heroes 2"];
#endif
	return [str cStringUsingEncoding:NSASCIIStringEncoding];
}


u_int32_t GetScreenWidth() {
#if defined(OS_MACOS)
    NSRect rect = [[NSScreen mainScreen] frame];
    return( rect.size.width );
#endif
}

u_int32_t GetScreenHeight(){
#if defined(OS_MACOS)
    NSRect rect = [[NSScreen mainScreen] frame];	
    return( rect.size.height );
#endif
}

u_int32_t GetWindowWidth() {
#if defined(OS_MACOS)	
//	NSRect rect = [[NSScreen mainScreen] frame];
//	return( rect.size.width );
	return macos_getCurScreenWidth();
#else
	CGRect bounds = [[UIScreen mainScreen] bounds];
	return fmaxf(bounds.size.height, bounds.size.width);
#endif
}

u_int32_t GetWindowHeight(){
#if defined(OS_MACOS)
//	NSRect rect = [[NSScreen mainScreen] frame];	
//	return( rect.size.height );
	return macos_getCurScreenHeight() - macos_offset();
#else
	CGRect bounds = [[UIScreen mainScreen] bounds];
	return fminf(bounds.size.height, bounds.size.width);
#endif
}

bool IsIpad()
{
    return UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad;
}

bool IsRetina()
{
	bool res = NO;
#if defined OS_IPHONE
	if ([UIScreen instancesRespondToSelector:@selector(scale)]) {
		CGFloat scale = [[UIScreen mainScreen] scale];
		if (scale > 1.0) {
			res = YES;
		}
	}
#endif
	return res;
}

#if defined(OS_IPHONE)
void OpenItunesLink() {
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString: @"itms://itunes.apple.com/us/app/palm-heroes-2-deluxe-for-iphone/id452997018?mt=8"]];
}
#endif


#ifdef OS_IPHONE
void OpenTutorialLink() {
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString: @"http://www.vimeo.com/truechromatic/palmheroestutorial"]];
}
#endif

FILE *fopen_iphone(const char *name, const char *right) {
/*	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
	char buf[1024];
    const char *s=[documentsDirectory cStringUsingEncoding:NSASCIIStringEncoding];
	strcpy(buf, s);
    strcat(buf, "/");
	strcat(buf, name);
    FILE *file = fopen(buf, right);
    if (file==NULL) {
        //LOG("failed to open file %s",fileName.c_str());
    }
    //LOG(s.c_str());
    return file;
*/
	return fopen(name, right);
}

