
#import <StoreKit/StoreKit.h>


@interface PHAppDelegate : UIApplication <SKProductsRequestDelegate> {
	UIWindow		*_window;
    int             _splash;
}

- (void) mainLoop: (id)param;

@property (nullable, nonatomic, strong) UIWindow* window;

@end

