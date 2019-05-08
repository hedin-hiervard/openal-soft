#ifndef RESLIB_ARTIFACTS_H_
#define RESLIB_ARTIFACTS_H_

//////////////////////////////////////////////////////////////////////////
class iMapItem_Artifact;
class iArtifactNode : public iBaseNode, public iIdxItem
{
public:
	IMPL_TYPEAWARE( iArtifactNode );
	iArtifactNode(const iStringT& id, const iStringT& nameKey, const iStringT& descKey, ART_TYPE type, ART_LEVEL_TYPE level, ART_ASSIGN_TYPE assign, uint32 reqLevel, SECONDARY_SKILLS reqSecSkill, const SpriteId& spriteId);
	iArtifactNode(RANDOM_ARTIFACTS type, const iStringT& id, const iStringT& nameKey, const SpriteId& spriteId);
	virtual ~iArtifactNode() {}

	iMapItem_Artifact* CreateObject(const iPoint& pos);
	virtual void Serialize(iDynamicBuffer& buff);

	inline const iStringT& DescKey() const { return m_descKey; }
	inline ART_TYPE Type() const { return m_Type; }
	inline ART_LEVEL_TYPE Level() const { return m_Level; }
	inline ART_ASSIGN_TYPE Assign() const { return m_assign; }
	inline const SpriteId& Sprite() const { return m_SpriteId; }
	inline bool IsRandom() const { return Idx() >= ARTT_RAND; }

private:
	iStringT			m_descKey;
	ART_TYPE			m_Type;
	ART_LEVEL_TYPE		m_Level;
	ART_ASSIGN_TYPE		m_assign;
	SpriteId			m_SpriteId;
	uint32				m_reqLevel;
	SECONDARY_SKILLS	m_reqSecSkill;
};

//
class iArtifactNode_FurtSkill : public iArtifactNode
{
public:
	iArtifactNode_FurtSkill(const iStringT& id, const iStringT& nameKey, const iStringT& descKey, ART_LEVEL_TYPE level, ART_ASSIGN_TYPE assign, uint32 reqLevel, SECONDARY_SKILLS reqSecSkill, const SpriteId& spriteId, const iFurtSkills& furtSkills, const iCreatBonus& cb);

	void Serialize(iDynamicBuffer& buff);

	inline const iFurtSkills& FurtSkills() const { return m_furtSkills; }

private:
	iFurtSkills m_furtSkills;
	iCreatBonus m_creatBonus;
};

class iArtifactNode_MagicSphere : public iArtifactNode
{
public:
	iArtifactNode_MagicSphere(const iStringT& id, const iStringT& nameKey, const iStringT& descKey, ART_LEVEL_TYPE level, ART_ASSIGN_TYPE assign, uint32 reqLevel, SECONDARY_SKILLS reqSecSkill, const SpriteId& spriteId, sint32 type);

	void Serialize(iDynamicBuffer& buff);

private:
	uint32 m_type;
};


//////////////////////////////////////////////////////////////////////////
class iMapItem_Artifact : public iMapItem
{
public:
	iMapItem_Artifact(const iPoint& pos, iArtifactNode* pProto);
	inline iArtifactNode* Proto() { return m_pProto; }

	virtual iBaseMapObject* Clone() { 
		iMapItem_Artifact *mi = new iMapItem_Artifact();
		*mi = *this;
		return mi;
	}

private:
	iArtifactNode*	m_pProto;
protected:
	iMapItem_Artifact() {}
};

#endif //RESLIB_ARTIFACTS_H_
