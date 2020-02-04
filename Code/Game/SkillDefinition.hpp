#pragma once

class ProjectileDef;
class PlayerController;

class SkillDefinition
{
public:
	SkillDefinition(ProjectileDef* def): m_pDef(def){}
	~SkillDefinition() {}

	void Cast(PlayerController* player);

private:
	ProjectileDef* m_pDef;
	float cooldown;
	float damageCoef;
};