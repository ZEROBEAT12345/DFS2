#pragma once
#include "Engine/Math/Vec2.hpp"
#include <string>

class Rigidbody2D;
class ProjectileDef;
class PlayerController;
class GPUMesh;
class Map;

enum SpecialProjectileTypeID
{
	PROJECTILE_NEWTON_NORMAL_ATTACK,
	PROJECTILE_NEWTON_SKILL_1,
	PROJECTILE_NEWTON_SKILL_2,
	PROJECTILE_JONES_NORMAL_ATTACK,
	PROJECTILE_JONES_SKILL_1,
	PROJECTILE_JONES_SKILL_2,
	PROJECTILE_TYPE_NUM
};

class Projectile
{
	friend Map;

public:
	Projectile(ProjectileDef* def, Map* map, Vec2 startPos = Vec2(0.f, 0.f), float forwardAngle = 0.f, float height = 0.f, int playerID = 0, SpecialProjectileTypeID pid = PROJECTILE_NEWTON_NORMAL_ATTACK) :
		m_def(def),
		m_themap(map),
		m_pos(startPos),
		m_forwardAngle(forwardAngle),
		m_height(height),
		m_playerID(playerID),
		m_pid(pid)
	{}

	~Projectile() {}

	void Initialize();
	void Update(float deltaSeconds);
	void Render();
	void Die();

	// Event Functions
	void OnHit();
	void OnOverlapWithPlayer(PlayerController* target);
	// TBD

	// Accessors
	Vec2 GetPos() const { return m_pos; }
	float GetForwardAngle() const { return m_forwardAngle; }
	int GetPlayerID() const { return m_playerID; }

public:
	ProjectileDef* m_def = nullptr;
	SpecialProjectileTypeID m_pid = PROJECTILE_NEWTON_NORMAL_ATTACK;

private:
	bool m_isDead = false;
	Vec2 m_pos = Vec2(0.f, 0.f);
	float m_forwardAngle = 0.f;
	float m_height = 0.f;
	float m_lifespan = 0.f;
	int m_playerID = 0;
	GPUMesh* m_mesh = nullptr;
	Map* m_themap = nullptr;

	// Newton - skill 1 - apple rain
	bool m_isAppleRainOn = false;
	float m_appleRainCount = 0.f;
	float m_appleRainMaxTime = 8.f;
	float m_appleRainRadius = 25.f;
	float m_appleRainDamage = 10.f;
	GPUMesh* m_appleRainRingMesh = nullptr;

	// Newton - skill 2 - dash
	bool m_isDashOn = true;
	float m_dashSpeed = 3.0f;

	// Jones - skill 1 - rope
	PlayerController* m_target = nullptr;
	bool m_isRopeFlying = true;
	float m_minDist = 11.f;

};