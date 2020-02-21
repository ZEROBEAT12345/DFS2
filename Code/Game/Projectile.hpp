#pragma once
#include "Engine/Math/Vec2.hpp"
#include <string>

class Rigidbody2D;
class ProjectileDef;
class GPUMesh;
class Map;

class Projectile
{
	friend Map;

public:
	Projectile(ProjectileDef* def, Vec2 startPos = Vec2(0.f, 0.f), float forwardAngle = 0.f, float height = 0.f, int playerID = 0) :
		m_def(def),
		m_pos(startPos),
		m_forwardAngle(forwardAngle),
		m_height(height),
		m_playerID(playerID)
	{}

	~Projectile() {}

	void Initialize();
	void Update(float deltaSeconds);
	void Render();
	void Die();

	// Event Functions
	// TBD

	// Accessors
	Vec2 GetPos() const { return m_pos; }
	float GetForwardAngle() const { return m_forwardAngle; }
	int GetPlayerID() const { return m_playerID; }

public:
	ProjectileDef* m_def = nullptr;

private:
	bool m_isDead = false;
	Vec2 m_pos = Vec2(0.f, 0.f);
	float m_forwardAngle = 0.f;
	float m_height = 0.f;
	float m_lifespan = 0.f;
	int m_playerID = 0;
	GPUMesh* m_mesh = nullptr;
	Rigidbody2D* m_rigidbody = nullptr;
};