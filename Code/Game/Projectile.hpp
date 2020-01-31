#pragma once
#include "Engine/Math/Vec3.hpp"
#include <string>

class ProjectileDef;

class Projectile
{
public:
	Projectile(ProjectileDef* def) : m_def(def) {}
	~Projectile();

	void Update(float deltaSeconds);
	void Render();

	void AddModel(std::string modelPath);

	// Event Functions
	// TBD

public:
	ProjectileDef* m_def = nullptr;

private:
	Vec3 m_pos;
	
};