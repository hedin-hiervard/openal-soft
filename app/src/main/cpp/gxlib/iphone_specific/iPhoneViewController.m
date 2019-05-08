//
//  iPhoneViewController.m
//  pheroes
//
//  Created by Hedin on 5/15/11.
//  Copyright 2011 Palm Heroes Team. All rights reserved.
//

#import "iPhoneViewController.h"
#import "iPhoneView.h"

@implementation iPhoneViewController


- (void)loadView {
	iPhoneView *view = [[iPhoneView alloc] initWithFrame: [UIScreen mainScreen].applicationFrame];
	self.view = view;
 	view.multipleTouchEnabled = YES;
    
    CADisplayLink* displayLink = [[UIScreen mainScreen] displayLinkWithTarget:view selector:@selector(updateSurface)];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
    [(iPhoneView*)self.view updateFrame];
}

// ios 5-

-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
    if(toInterfaceOrientation == UIInterfaceOrientationLandscapeLeft ||
            toInterfaceOrientation == UIInterfaceOrientationLandscapeRight)
        return YES;
    return NO;
}

//- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
//	return (interfaceOrientation == UIInterfaceOrientationLandscapeLeft ||
//			interfaceOrientation == UIInterfaceOrientationLandscapeRight);
//}

// IOS 6+
- (BOOL)shouldAutorotate
{
    return TRUE;    
}

// IOS 6+
- (NSUInteger)supportedInterfaceOrientations
{
    //decide number of origination tob supported by Viewcontroller.
    return UIInterfaceOrientationMaskLandscape;        
}


@end
