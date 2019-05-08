#ifndef RESLIB_SOUNDMGR_H_
#define RESLIB_SOUNDMGR_H_

class iSoundMgr
{
public:
	struct iSoundItem {
		iSoundItem(const iStringT& _fileName, const iStringT& _soundId, const iStringT& _constId)
		: fileName(_fileName), soundId(_soundId), constId(_constId) {}
		iStringT	fileName;
		iStringT	soundId;
		iStringT	constId;
	};
public:
	iSoundMgr();
	~iSoundMgr();
	bool Init(const iStringT& fileName);
	bool AddSoundItem(const iStringT& fileName, const iStringT& soundId, const iStringT& constId);
	uint32 Count() const { return m_items.GetSize(); }
	sint32 FindItem(const iStringT& soundId) const;
	const iSoundItem& Item(uint32 idx) const { return m_items[idx]; }

private:
	iStringT					m_resPath;
	iSimpleArray<iSoundItem>	m_items;

};

#endif //RESLIB_SOUNDMGR_H_
