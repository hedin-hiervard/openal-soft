#ifndef PHEROES_GAME_CASTLEVIEW_STYLUS_H_
#define PHEROES_GAME_CASTLEVIEW_STYLUS_H_
#pragma once

class iCastleView_stylus : public iCastleView 
{
public:
	iCastleView_stylus();
	bool OnKeyDown(sint32 key);		
private:
	void OnCompose();
private:
	
};	
#endif //PHEROES_GAME_CASTLEVIEW_STYLUS_H_
