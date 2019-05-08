#ifndef GXLIB_IPHONE_SPECIFIC_IPHONE_KEYBOARD_H_
#define GXLIB_IPHONE_SPECIFIC_IPHONE_KEYBOARD_H_
#import <UIKit/UIKit.h>
#import <UIKit/UITextView.h>

@interface SoftKeyboard : UIView {
	id inputDelegate;
	UITextView* inputView;
}

- (id)initWithFrame:(CGRect)frame;
- (UITextView*)inputView;
- (void)setInputDelegate:(id)delegate;
- (void)handleKeyPress:(unichar)c;

@end
#endif //GXLIB_IPHONE_SPECIFIC_IPHONE_KEYBOARD_H_
