#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
void iAvatarsHolder::AddAvatar(uint16 aid, sint16 ox, sint16 oy)
{ 
	// Add Avatar
	m_Avatars.Add(iAvatarEntry(aid, ox,oy)); 
	// Add Cover entry
	const iAvatar& avatar = gGame.ItemMgr().m_AvatarMgr[aid];
	for (uint32 pmid=0; pmid<avatar.PassModsCount(); ++pmid) {
		m_Covers.AddCover(ox+avatar.PassModEntry(pmid).ox,oy+avatar.PassModEntry(pmid).oy);
	}
}

/*
 *	
 */
iAvatarMgr::iAvatarMgr()
{
}

iAvatarMgr::~iAvatarMgr()
{
	uint32 cnt = m_Avatars.GetSize();
	while (cnt--) {
		delete m_Avatars[cnt];
	}
	m_Avatars.RemoveAll();
}

iAvatar& iAvatarMgr::AddAvatar(TextResId nameKey)
{
	iAvatar* pRes = new iAvatar(nameKey);
	m_Avatars.Add(pRes);
	return *pRes;
}

uint16 iAvatarMgr::FindAvatarBySprite(SpriteId sid) const
{
	for (uint16 xx=0; xx<m_Avatars.GetSize(); ++xx) {
		if (m_Avatars[xx]->SpritesCount() == 1 && m_Avatars[xx]->SpriteEntry(0).sid == sid) return xx;
	}
	return 0xFFFF;
}

/*
 *	
 */
bool LoadAvatars(iDynamicBuffer& buff, iAvatarMgr &amgr)
{
	uint16 acount;
	buff.Read(acount);
	while (acount--) {
		// Name
		sint16 avtNameKey;
		buff.Read(avtNameKey);

		// Create instance
		iAvatar& avatar = amgr.AddAvatar(avtNameKey);

		// sound
		uint16 soundId;
		buff.Read(soundId);
		avatar.SetSoundId(soundId);

		// sprites
		uint8 count;
		buff.Read(count);
		while (count--) {
			uint16 sid;
			sint16 ox,oy;
			uint8 level;
			buff.Read(sid);
			buff.Read(level);
			buff.Read(ox);
			buff.Read(oy);
			avatar.AddSprite(sid,ox,oy,(SPRITE_LEVEL)level);
		}
		// Covers
		buff.Read(count);
		while (count--) {
			uint8 mod;
			sint16 ox,oy;
			buff.Read(mod);
			buff.Read(ox);
			buff.Read(oy);
			avatar.AddPassMod(ox,oy,mod);
		}
		// Flags
		buff.Read(count);
		sint16 ox,oy;
		while (count--) {
			buff.Read(ox);
			buff.Read(oy);
			avatar.AddFlag(ox,oy);
		}
	}

	return true;
}


