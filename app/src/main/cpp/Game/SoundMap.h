#ifndef PHEROES_GAME_SOUNDMAP_H_
#define PHEROES_GAME_SOUNDMAP_H_

class iSoundMap
{
public:
	iSoundMap();
	~iSoundMap();

	bool Init(iGameWorld* pMap);
	void Cleanup();
	void Update();

	void SetupEnvSounds(const iPoint& pos);
	void ResetEnvSounds();

private:
	struct iSndEnvItem {
		iSndEnvItem(uint16 _idx, uint16 _delta = 0, sint32 _channel = -1) : idx(_idx), delta(_delta), channel(_channel) {}
		
		bool operator == (const iSndEnvItem& other) const { return idx == other.idx; }
		bool operator != (const iSndEnvItem& other) const { return idx != other.idx; }

		uint16	idx;
		uint16	delta;
		sint32	channel;
	};

	typedef iSimpleArray<iSndEnvItem> iItemList;

private:
	iItemList		m_items;
	iGameWorld*		m_pMap;
	iMapT<uint16>	m_sndMap;
};


#endif //PHEROES_GAME_SOUNDMAP_H_
