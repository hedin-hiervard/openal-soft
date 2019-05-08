#ifndef RESLIB_RESMGR_H_
#define RESLIB_RESMGR_H_


/*
 *	
 */
inline sint32 FindStr(LPCTSTR *pStrList, uint32 count, LPCTSTR pSubStr)
{
	for (uint32 xx=0; xx<count; ++xx){
		if (_tcsicmp(pStrList[xx],pSubStr) == 0) return xx;
	}
	return -1;
}

/*
 *	
 */
class iObjGroupNode : public iGroupNode
{
public:
	iObjGroupNode(const iStringT& name)
	: iGroupNode(name)
	{}

	iArtifactNode* GetArtifactNode(const iStringT& id)
	{
		Iterator it = First();
		while (*it) {
			if (iArtifactNode* pArtifact = DynamicCast<iArtifactNode*>((iBaseNode*)*it)) {
				if (pArtifact->Id() == id) return pArtifact;
			}
			it = it.WalkNext();
		}
		return NULL;
	}

	iArtifactNode* GetArtifactNode(uint16 idx)
	{
		Iterator it = First();
		while (*it) {
			if (iArtifactNode* pArtifact = DynamicCast<iArtifactNode*>((iBaseNode*)*it)) {
				if (pArtifact->Idx() == idx) return pArtifact;
			}
			it = it.WalkNext();
		}
		return NULL;
	}
	
	iDecorNode* GetDecorNode(const iStringT& id)
	{
		Iterator it = First();
		while (*it) {
			if (iDecorNode* pDecor = DynamicCast<iDecorNode*>((iBaseNode*)*it)) {
				if (pDecor->Id() == id) return pDecor;
			}
			it = it.WalkNext();
		}
		return NULL;
	}

	iPathElNode* GetPathElNode(const iStringT& id)
	{
		Iterator it = First();
		while (*it) {
			if (iPathElNode* pPathEl = DynamicCast<iPathElNode*>((iBaseNode*)*it)) {
				if (pPathEl->Id() == id) return pPathEl;
			}
			it = it.WalkNext();
		}
		return NULL;
	}

	iBaseOwnCnstNode* GetOwnCnstNode(const iStringT& id)
	{
		Iterator it = First();
		while (*it) {
			if (iBaseOwnCnstNode* pOwnCnst = DynamicCast<iBaseOwnCnstNode*>((iBaseNode*)*it)) {
				if (pOwnCnst->Id() == id) return pOwnCnst;
			}
			it = it.WalkNext();
		}
		return NULL;
	}

	iCastleNode* GetCastleNode(const iStringT& id, CTL_TYPE type)
	{
		Iterator it = First()+type;
		for (Iterator cit = (*it)->First(); cit != (*it)->End(); ++cit) {
			iCastleNode* pCastle = DynamicCast<iCastleNode*>((iBaseNode*)*cit);
			check(pCastle);
			if (pCastle->Id() == id) return pCastle;
		}
		/*
		while (*it) {
			if (iCastleNode* pCastle = DynamicCast<iCastleNode*>((iBaseNode*)*it)) {
				if (pCastle->Id() == id && pCastle->Type() == type) return pCastle;
			}
			it = it.WalkNext();
		}
		*/
		return NULL;
	}	

	iBaseVisCnstNode* GetVisCnstNode(const iStringT& id)
	{
		Iterator it = First();
		while (*it) {
			if (iBaseVisCnstNode* pVisCnst = DynamicCast<iBaseVisCnstNode*>((iBaseNode*)*it)) {
				if (pVisCnst->Id() == id) return pVisCnst;
			} else if (iVisCnstNode_NewTeleport* pVisCnst = DynamicCast<iVisCnstNode_NewTeleport*>((iBaseNode*)*it)) {
				if (pVisCnst->Id() == id) return pVisCnst;
			}
			it = it.WalkNext();
		}
		return NULL;
	}

	iHeroNode* GetHeroNode(const iStringT& id)
	{
		Iterator it = First();
		while (*it) {
			if (iHeroNode* pHeroNode = DynamicCast<iHeroNode*>((iBaseNode*)*it)) {
				if (pHeroNode->Id() == id) return pHeroNode;
			}
			it = it.WalkNext();
		}
		return NULL;
	}
    
    iShipNode* GetShipNode(const iStringT& id)
    {
        Iterator it = First();
        while (*it) {
            if (iShipNode* pShipNode = DynamicCast<iShipNode*>((iBaseNode*)*it))
            {
                if (pShipNode->Id() == id) return pShipNode;
            }
            it = it.WalkNext();
        }
        return NULL;
    }
};

/*
 *	
 */

class ZipItem {
public:
	iStringT path;
	char* buffer;
	int length;
};

class iItemMgr : public iTreeNode
{
public:
	iItemMgr()
	: m_bInited(false), m_bPacked(false) 
	{}

	~iItemMgr();

	bool Init(const iStringT& fname, const iStringT &path, bool packed);
	bool IsPacked() {return m_bPacked;}

	void Reset();

	iObjGroupNode*			m_pResNode;
	iObjGroupNode*			m_pArtifactNode;
	iObjGroupNode*			m_pDecorNode;
	iObjGroupNode*			m_pPathElNode;
	iObjGroupNode*			m_pVisCnstNode;
	iObjGroupNode*			m_pOwnCnstNode;
	iObjGroupNode*			m_pCastleNode;
	iObjGroupNode*			m_pHeroesNode;
    iObjGroupNode*			m_pShipsNode;
	iObjGroupNode*			m_pGuardsNode;
	iStringT				m_SetName;
	iTextMgr				m_TextMgr;
	iSpriteMgr				m_SpriteMgr;
	iSoundMgr				m_SoundMgr;
	iAvatarMgr				m_AvatarMgr;

	iSimpleArray<ZipItem>  m_ZipItems;
	char* GetZipBuffer(iStringT name, int& length); 

	iSimpleArray<iBattleObstacle>	m_battleObstacles;	
	iSimpleArray<iStringT> m_fontFiles;
	iSimpleArray<iPair<iStringT, bool> > m_pixFiles;

private:
	bool	m_bInited;
	bool	m_bPacked;
};



#endif //RESLIB_RESMGR_H_
