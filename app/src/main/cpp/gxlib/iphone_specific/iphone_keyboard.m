#import "iphone_keyboard.h"

@interface UITextInputTraits
- (void)setAutocorrectionType:(int)type;
- (void)setAutocapitalizationType:(int)type;
- (void)setEnablesReturnKeyAutomatically:(BOOL)val;
@end

@interface TextInputHandler : UITextView {
	SoftKeyboard* softKeyboard;
}

- (id)initWithKeyboard:(SoftKeyboard*)keyboard;

@end


@implementation TextInputHandler

- (id)initWithKeyboard:(SoftKeyboard*)keyboard; {
	self = [super initWithFrame:CGRectMake(0.0f, 0.0f, 0.0f, 0.0f)];
	softKeyboard = keyboard;
	
//	[[self textInputTraits] setAutocorrectionType:1];
//	[[self textInputTraits] setAutocapitalizationType:0];
//	[[self textInputTraits] setEnablesReturnKeyAutomatically:NO];
	
	return self;
}

- (void) keyboardInputShouldDelete:(id)input {
	[softKeyboard handleKeyPress:0x08];
}

- (BOOL)webView:(id)fp8 shouldInsertText:(id)character
                       replacingDOMRange:(id)fp16
                             givenAction:(int)fp20 {

	if ([character length] != 1) {
		[NSException raise:@"Unsupported" format:@"Unhandled multi-char insert!"];
		return NO;
	}

	[softKeyboard handleKeyPress:[character characterAtIndex:0]];
	
	return NO;
}

@end


@implementation SoftKeyboard

- (id)initWithFrame:(CGRect)frame {
	self = [super initWithFrame:frame];
	inputDelegate = nil;
	inputView = [[TextInputHandler alloc] initWithKeyboard:self];
	return self;
}

- (UITextView*)inputView {
	return inputView;
}

- (void)setInputDelegate:(id)delegate {
	inputDelegate = delegate;
}

- (void)handleKeyPress:(unichar)c {
	[inputDelegate handleKeyPress:c];
}

@end
