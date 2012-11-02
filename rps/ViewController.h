//
//  ViewController.h
//  rps
//
//  Created by Kyle Weicht on 7/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

@interface ViewController : GLKViewController

@property game_t* game;

@end

float get_device_scale(void);
float get_device_width(void);
float get_device_height(void);
