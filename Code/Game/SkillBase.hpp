#pragma once



class Skill
{
public:
	Skill() {}
	virtual ~Skill();

	virtual void Cast();

protected:
	float cooldown;
	float damageCoef;
};