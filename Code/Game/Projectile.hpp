#pragma once
#include "Engine/Math/Vec3.hpp"
#include <string>

struct ProjectileInfo
{
	float velocity;
	float existTime;
	float damageCoef;
};

class Projectile
{
public:
	Projectile();
	virtual ~Projectile();

	virtual void Update(float deltaSeconds);
	virtual void Render();

	void AddModel(std::string modelPath);

	// Event Functions
	// TBD

private:
	Vec3 m_pos;
	
};