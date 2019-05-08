#ifndef PHEROES_GAME_RMG_H_
#define PHEROES_GAME_RMG_H_
#pragma once

class iRandomMapGenerator {
public:
	struct iRandomMapParam {
		DIFFICULTY_LEVEL			m_Difficulty;
		iSimpleArray<iMapInfo::iPlayerInfo>	m_Players;
		MAP_SIZE					m_Size;
		iRandomMapParam();
	};

	void Generate(const iRandomMapParam &param);
};
#endif //PHEROES_GAME_RMG_H_
