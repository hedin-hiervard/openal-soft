#include <stdafx.h>
#include "cnst_text.h" 

extern LPCWSTR LANG_DATA_ENG[1618];
extern LPCWSTR LANG_DATA_RUS[1618];
extern LPCWSTR LANG_DATA_SLOV[1618];
extern LPCWSTR LANG_DATA_GER[1618];
extern LPCWSTR LANG_DATA_CZE[1618];
extern LPCWSTR LANG_DATA_FR[1618];
extern LPCWSTR LANG_DATA_PL[1618];

LPCWSTR *LANG_DATA = LANG_DATA_ENG;
LPCWSTR *LANG_DATA_ENGLISH = LANG_DATA_ENG;

void SetGameLanguage(GAME_LANG lng)
{
	if(lng == GLNG_RUSSIAN) 
		LANG_DATA = LANG_DATA_RUS;
	else if(lng == GLNG_ENGLISH) 
		LANG_DATA = LANG_DATA_ENG;
	else if(lng == GLNG_SLOVAK) 
		LANG_DATA = LANG_DATA_SLOV;
	else if(lng == GLNG_GERMAN) 
		LANG_DATA = LANG_DATA_GER;
	else if(lng == GLNG_CZECH) 
		LANG_DATA = LANG_DATA_CZE;
	else if(lng == GLNG_FRENCH) 
		LANG_DATA = LANG_DATA_FR;
	else if(lng == GLNG_POLISH) 
		LANG_DATA = LANG_DATA_PL;	
	gSettings.SetLanguage(lng);	

}