//
//  iPhoneViewController.m
//  pheroes
//
//  Created by Hedin on 5/15/11.
//  Copyright 2011 Palm Heroes Team. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SplashViewController.h"

@implementation SplashViewController



- (void)loadView {
    NSString* filename = [NSString stringWithFormat: @"splash-%@.png", _splashName];
    UIImageView* view = [[UIImageView alloc] initWithImage: [UIImage imageNamed: filename]];
    self.view = view;
    view.backgroundColor = [UIColor whiteColor];
}

-(id) initWithSplashName: (NSString*) splashName
{
    id cont = [super init];
    _splashName = splashName;
    return cont;
}

@end
