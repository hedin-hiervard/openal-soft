//
//  iPhoneViewController.h
//  pheroes
//
//  Created by Hedin on 5/15/11.
//  Copyright 2011 Palm Heroes Team. All rights reserved.
//

#import <UIKit/UIKit.h>

@class SplashView;

@interface SplashViewController : UIViewController {
    NSString* _splashName;
}

-(id) initWithSplashName: (NSString*) splashName;


@end
