#ifndef PHEROES_GAME_SPLASH_H_
#define PHEROES_GAME_SPLASH_H_
/*
 *  Splash.h
 *  pheroes
 *
 *  Created by hedin on 6/9/10.
 *  Copyright 2010 Palm Heroes Team. All rights reserved.
 *
 */

class iSplash {
public:
	void Show();
	void Disappear();
private:
	iDib back;
};

#endif //PHEROES_GAME_SPLASH_H_
