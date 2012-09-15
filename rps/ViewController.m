//
//  ViewController.m
//  rps
//
//  Created by Kyle Weicht on 7/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "ViewController.h"

#include "render.h"
#include "game.h"
#include "system.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

@interface ViewController () {
    GLuint _program;
    
    GLuint _vertexArray;
    GLuint _vertexBuffer;
}
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation ViewController

@synthesize context = _context;
@synthesize game = _game;

- (void)viewDidLoad
{
    UITapGestureRecognizer *tap = nil;
    GLKView *view = nil;
    
    [super viewDidLoad];
    self.preferredFramesPerSecond = 60;
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    [self setupGL];
    
    tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTap:)];
    tap.numberOfTapsRequired = 1;
    [[self view] addGestureRecognizer:tap];

    [[self view] setMultipleTouchEnabled:TRUE];
}
- (void)handleTap:(UIGestureRecognizer *)sender
{
    CGPoint tapPoint = [sender locationInView:nil];
    tapPoint.x *= get_device_scale();
    tapPoint.y *= get_device_scale();
    if([[UIDevice currentDevice] orientation] == UIInterfaceOrientationPortraitUpsideDown) {
        // The OS should really rotate the taps for us :-(
        tapPoint.y = get_device_height() - tapPoint.y;
        tapPoint.x = get_device_width() - tapPoint.x;
    }
    game_handle_tap(_game, tapPoint.x, tapPoint.y);
}
- (void)viewDidUnload
{    
    [super viewDidUnload];
    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
	self.context = nil;
    
    game_shutdown(_game);
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc. that aren't in use.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // No landscape for us!
    if(interfaceOrientation == UIInterfaceOrientationPortraitUpsideDown ||
       interfaceOrientation == UIInterfaceOrientationPortrait )
    {
        return YES;
    }
    return NO;
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    game_update(_game);
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    game_render(_game);
}
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    for(UITouch *touch in touches) {
        CGPoint tapPoint = [touch locationInView:nil];
        tapPoint.x *= get_device_scale();
        tapPoint.y *= get_device_scale();
        if([[UIDevice currentDevice] orientation] == UIInterfaceOrientationPortraitUpsideDown) {
            // The OS should really rotate the taps for us :-(
            tapPoint.y = get_device_height() - tapPoint.y;
            tapPoint.x = get_device_width() - tapPoint.x;
        }
        game_handle_touch(_game, tapPoint.x, tapPoint.y);
        break;
    }
}
 
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    for(UITouch *touch in touches) {
        CGPoint tapPoint = [touch locationInView:nil];
        tapPoint.x *= get_device_scale();
        tapPoint.y *= get_device_scale();
        if([[UIDevice currentDevice] orientation] == UIInterfaceOrientationPortraitUpsideDown) {
            // The OS should really rotate the taps for us :-(
            tapPoint.y = get_device_height() - tapPoint.y;
            tapPoint.x = get_device_width() - tapPoint.x;
        }
        game_handle_touch(_game, tapPoint.x, tapPoint.y);
        break;
    }
}
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    game_clear_touch(_game);
}
 
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    game_clear_touch(_game);
}

@end

