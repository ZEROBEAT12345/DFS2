#pragma once

class Map;
class ProjectileDef;
class PlayerController;

class SkillDefinition
{
public:
	SkillDefinition(ProjectileDef* def): m_pDef(def){}
	~SkillDefinition() {}

	void Cast(PlayerController* player, Map* curMap);

private:
	ProjectileDef* m_pDef;
	float cooldown = 3.f;
	float damageCoef = 1.f;
};