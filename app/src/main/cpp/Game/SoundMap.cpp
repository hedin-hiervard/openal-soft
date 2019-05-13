#include "stdafx.h"
#include "SoundMap.h"

#include "FileAccessor/FileAccessor.h"

using namespace fileaccessor;

//////////////////////////////////////////////////////////////////////////
iSoundMap::iSoundMap()
: m_pMap(NULL)
{

}

iSoundMap::~iSoundMap()
{
	Cleanup();
}

bool iSoundMap::Init(iGameWorld* pMap)
{
	m_pMap = pMap;
	m_sndMap.Init(m_pMap->GetWidth(), m_pMap->GetHeight());
	Update();

	return true;
}

void iSoundMap::Cleanup()
{
	ResetEnvSounds();
	m_pMap = NULL;
	m_sndMap.Cleanup();
}


void iSoundMap::Update()
{
	check(m_pMap);

	m_sndMap.FillMem(0xFFFF);

	// Surface and avatars
	for (sint32 yy=0; yy<m_sndMap.GetHeight(); ++yy) {
		for (sint32 xx=0; xx<m_sndMap.GetWidth(); ++xx) {
			iGameMap::iSurfCell cell;
			m_pMap->GetCellSurf(iPoint(xx, yy), cell);
			uint16 avatarId = m_pMap->GetAt(xx,yy).avatar;
			if (avatarId != 0xFFFF ) {
				if ( (avatarId & 0xFF00) == 0xFF00) {
					// path element
				} else {
					// avatar
					const iAvatar& avatar = gGame.ItemMgr().m_AvatarMgr[avatarId];
					uint16 soundId = avatar.GetSoundId();
					if (soundId != 0xFFFF) {
						for (uint32 pmid=0; pmid<avatar.PassModsCount(); ++pmid) {
							sint32 ox = xx+avatar.PassModEntry(pmid).ox;
							sint32 oy = yy+avatar.PassModEntry(pmid).oy;
							if (m_sndMap.IsValidPos(ox,oy)) m_sndMap.GetAt(ox,oy) = soundId;
						}
					}
				}
			} else 	if (cell.lowestLayer == STYPE_WATER) m_sndMap.GetAt(xx, yy) = CSND_SEAWASH;
		}
	}

	// Visitables
	for (iGameMap::iVCIt vcit = m_pMap->m_VisCnsts.First(); vcit != m_pMap->m_VisCnsts.End(); ++vcit) {
		iVisCnst* pCnst = *vcit;
		const iVisCnstT* pProto = pCnst->Proto();
		for (uint32 aid = 0; aid<pProto->AvatarsCount(); ++aid) {
			const iAvatar& avatar = gGame.ItemMgr().m_AvatarMgr[pProto->AvatarEntry(aid).aid];
			uint16 soundId = avatar.GetSoundId();
			if (soundId != 0xFFFF) {
				for (uint32 pmid=0; pmid<avatar.PassModsCount(); ++pmid) {
					sint32 ox = pCnst->Pos().x+pProto->AvatarEntry(aid).ox+avatar.PassModEntry(pmid).ox;
					sint32 oy = pCnst->Pos().y+pProto->AvatarEntry(aid).oy+avatar.PassModEntry(pmid).oy;
					if (m_sndMap.IsValidPos(ox,oy)) m_sndMap.GetAt(ox,oy) = soundId;
					//OutputDebugString(iFormat(_T("Insert sound at %d,%d\n"), ox, oy).CStr());
				}
			}
		}
	}

	// Neutral ownerables
	for (iGameMap::iOCIt ocit = m_pMap->m_OwnCnsts.First(); ocit != m_pMap->m_OwnCnsts.End(); ++ocit) {
		iOwnCnst* pCnst = *ocit;
		const iOwnCnstT* pProto = pCnst->Proto();
		for (uint32 aid = 0; aid<pProto->AvatarsCount(); ++aid) {
			const iAvatar& avatar = gGame.ItemMgr().m_AvatarMgr[pProto->AvatarEntry(aid).aid];
			uint16 soundId = avatar.GetSoundId();
			if (soundId != 0xFFFF) {
				for (uint32 pmid=0; pmid<avatar.PassModsCount(); ++pmid) {
					sint32 ox = pCnst->Pos().x+pProto->AvatarEntry(aid).ox+avatar.PassModEntry(pmid).ox;
					sint32 oy = pCnst->Pos().y+pProto->AvatarEntry(aid).oy+avatar.PassModEntry(pmid).oy;
					if (m_sndMap.IsValidPos(ox,oy)) m_sndMap.GetAt(ox,oy) = soundId;
					//OutputDebugString(iFormat(_T("Insert sound at %d,%d\n"), ox, oy).CStr());
				}
			}
		}
	}

	for (iGameMap::iPLIt pit = m_pMap->m_Players.First(); pit != m_pMap->m_Players.End(); ++pit) {
		// Ownerables
		for (iGameMap::iOCIt ocit = (*pit)->m_Cnsts.First(); ocit != (*pit)->m_Cnsts.End(); ++ocit) {
			iOwnCnst* pCnst = *ocit;
			const iOwnCnstT* pProto = pCnst->Proto();
			for (uint32 aid = 0; aid<pProto->AvatarsCount(); ++aid) {
				const iAvatar& avatar = gGame.ItemMgr().m_AvatarMgr[pProto->AvatarEntry(aid).aid];
				uint16 soundId = avatar.GetSoundId();
				if (soundId != 0xFFFF) {
					for (uint32 pmid=0; pmid<avatar.PassModsCount(); ++pmid) {
						sint32 ox = pCnst->Pos().x+pProto->AvatarEntry(aid).ox+avatar.PassModEntry(pmid).ox;
						sint32 oy = pCnst->Pos().y+pProto->AvatarEntry(aid).oy+avatar.PassModEntry(pmid).oy;
						if (m_sndMap.IsValidPos(ox,oy)) m_sndMap.GetAt(ox,oy) = soundId;
						//OutputDebugString(iFormat(_T("Insert sound at %d,%d\n"), ox, oy).CStr());
					}
				}
			}
		}
	}
}

const sint32 VOL_PRESETS[5] = {
	25,
	20,
	12,
	6,
	3,
};

std::string SURF_MUSIC[STYPE_COUNT] = {
	(""),	// water
	("Expeditionary"),	// sand (beach)
	("Colossus"),	// dirt
	("Arid Foothills"),	// grass
	("Opium"),	// swamp
	("Supernatural"),	// lava
	("Willow and the Light"),	// wasteland
	("Desert City"),	// desert
	("Temple of the Manes"),	// snow
	("Cambodian Odyssey"),	// new desert
	("Celtic Impulse"),	// pavement
	("Phantasm"),	// new wasteland
};

std::string nowPlaying = ("");

void iSoundMap::SetupEnvSounds(const iPoint& pos)
{
	// background music
	iCell c = gGame.Map().GetAt(pos.x,pos.y);
	SURF_TYPE upSurf = (SURF_TYPE)c.SurfNode(0);
	if(nowPlaying != SURF_MUSIC[upSurf])
	{
        if(  SURF_MUSIC[upSurf] == ("") )
            return;

		nowPlaying = SURF_MUSIC[upSurf];
		auto path = RelativeFilePath(std::string("Music/") + SURF_MUSIC[upSurf] + (".mp3"));
		gMusicMgr.Play(path, true);
	}

	if (!gApp.SndPlayer().Inited()) return;
	iItemList newEnvItems;
	check(m_pMap->IsValidPos(pos.x, pos.y));

	// Clip env rect area
	iRect area(pos.x-4, pos.y-4, 9, 9);
	iClipper::ClipRect(area, iRect(0,0,m_pMap->GetWidth(), m_pMap->GetHeight()));

	// Prepare new env list
	for (sint32 yy=area.y; yy<=area.y2(); ++yy) {
		for (sint32 xx=area.x; xx<=area.x2(); ++xx) {
			uint16 snd = m_sndMap.GetAt(xx, yy);
			if (snd != 0xFFFF) {
				uint16 delta = iMAX<uint16>(iDIF(pos.x, xx), iDIF(pos.y, yy));
				sint32 found = newEnvItems.Find(iSndEnvItem(snd));
				if (found != iItemList::NOT_FOUND) {
					if (newEnvItems[found].delta > delta) newEnvItems[found].delta = delta;
				} else {
					newEnvItems.Add(iSndEnvItem(snd, delta));
				}
			}
		}
	}

	// Stop old sounds
	for (uint32 nn=0; nn<m_items.GetSize();) {
		if (newEnvItems.Find(m_items[nn]) == iItemList::NOT_FOUND) {
			gApp.SndPlayer().Stop(m_items[nn].channel);
			m_items.RemoveAt(nn);
		} else ++nn;
	}

	// Add new and modify volume for existed
	for (uint32 nn=0; nn<newEnvItems.GetSize(); ++nn) {
		sint32 found = m_items.Find(newEnvItems[nn]);
		if (found == iItemList::NOT_FOUND) {
			//uint16 aa = newEnvItems[nn].idx;
			// Add new sound
			newEnvItems[nn].channel = gSfxMgr.PlaySound(newEnvItems[nn].idx, true);
			if(newEnvItems[nn].channel != -1) {
				m_items.Add(newEnvItems[nn]);
				// Change volume
				gApp.SndPlayer().SetStreamVolume(newEnvItems[nn].channel, VOL_PRESETS[newEnvItems[nn].delta]);
			}
		} else {
			if (m_items[found].delta != newEnvItems[nn].delta) {
				m_items[found].delta = newEnvItems[nn].delta;
				gApp.SndPlayer().SetStreamVolume(m_items[found].channel, VOL_PRESETS[m_items[found].delta]);
			}
		}
	}
}

void iSoundMap::ResetEnvSounds()
{
	nowPlaying = ("");
	if (!gApp.SndPlayer().Inited()) return;
	for (uint32 xx=0; xx<m_items.GetSize(); ++xx) {
		check(m_items[xx].channel != -1);
		gApp.SndPlayer().Stop(m_items[xx].channel);
	}
	m_items.RemoveAll();
}

