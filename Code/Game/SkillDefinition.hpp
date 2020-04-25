#pragma once

class Map;
class ProjectileDef;
class PlayerController;

enum SkillType
{
	SKILL_NEWTON_NORMAL_ATTACK,
	SKILL_NEWTON_SKILL_1,
	SKILL_NEWTON_SKILL_2,
	SKILL_JONES_NORMAL_ATTACK,
	SKILL_JONES_SKILL_1,
	SKILL_JONES_SKILL_2,
	SKILL_TYPE_NUM
};

class SkillDefinition
{
public:
	SkillDefinition(ProjectileDef* def): m_pDef(def){}
	~SkillDefinition() {}

	void SetSkillType(SkillType type) { m_skillType = type; }
	void Cast(PlayerController* player, Map* curMap);

public:
	SkillType m_skillType = SKILL_NEWTON_NORMAL_ATTACK;
	ProjectileDef* m_pDef;
	float cooldown = 3.f;
	float damageCoef = 1.f;

	// other skill counter

};