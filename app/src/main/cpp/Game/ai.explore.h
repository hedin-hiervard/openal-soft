#ifndef PHEROES_GAME_AI_EXPLORE_H_
#define PHEROES_GAME_AI_EXPLORE_H_

class iExplorePoint
{
public:
	iExplorePoint(iHero& hero);
	sint32 FindExplorePoint(sint32 radius, iPoint& target);
	void EnumAllExplorePoints(iShPointList& targets);

private:
	bool ScanRadius(sint32 radius, iShPointList& scanRes, iPlayer::iPassMap& passMap) const;

private:
	iHero&					m_hero;
	iPoint					m_pos;
	const iFogMap&			m_fogMap;
	iPlayer::iPassMap		m_passMap;
};


#endif //PHEROES_GAME_AI_EXPLORE_H_
