#ifndef PHEROES_GAME_HELPERS_H_
#define PHEROES_GAME_HELPERS_H_


sint32 InitialTroopQuantity(CREATURE_TYPE ct);
uint32 SelectSpells(iSpellList& spIds, iSpellFilter filter, uint32 count, MAGIC_SPELL reqSpell);
sint32 CalcRandValue(const uint8 pVals[], sint32 siz);
sint32 InitialExpPts();
iStringT FormatDate(uint32 timestamp, bool bShowTime);
iStringT FormatTime(uint32 timestamp);
iStringT FormatDateDM(uint32 timestamp, bool bShowTime);
sint32 GetRectsHeight(sint32 w, iSimpleArray<iSize>& szs);
iPoint GetRectOffset(sint32 w, uint32 idx, iSimpleArray<iSize>& szs);

/// helper to save game to a file
bool SaveGameFile( const iStringT& fname, iGameWorld& gmap, bool MP = false );

/// Returns the platform-independent timestamp
uint32 GetCurrentTimestamp();

#endif //PHEROES_GAME_HELPERS_H_
