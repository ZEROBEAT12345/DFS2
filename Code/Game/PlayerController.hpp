#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Physics/Transform2D.hpp"
#include "Game/Entity.hpp"

class VoxelMesh;
class GPUMesh;
class Map;
class SkillDefinition;
class VoxelAnimator;
class VoxelAnimDef;
class Rigidbody2D;
class Game;

enum SkillID
{
	SKILL_NORMAL_ATTACK,
	SKILL_1,
	SKILL_2,
	SKILL_ULTIMATE,
	SKILL_NUM
};

struct PlayerAttrib
{
	int maxHealth = 100;
	float movingSpeed = 20.f;
	int attack;
	int armor;
	float colliderSize = 5.f;
	std::string SkillID_1;
	std::string SkillID_2;
	std::string SkillID_3;
	std::string SkillID_4;
}; 

class PlayerController: public Entity
{
	friend SkillDefinition;
	friend Map;

public:
	PlayerController(int controllerID, Map* map, Game* game, PlayerAttrib attrib) : m_controllerID(controllerID), m_curMap(map), m_game(game), m_attribe(attrib) {}
	~PlayerController();

	void BeginFrame();
	void Update(float deltaSeconds) override;
	void Render() const override;

	void Initialize();
	void Die();
	void AddModel(std::string bodyModel, std::string handModel);
	void AddSkill(SkillDefinition* skill, int skillID) { m_skills[skillID] = skill; }
	void AddDamagedAnim(VoxelAnimDef* anim) { m_damagedAnim = anim; };
	void AddAttackAnim(VoxelAnimDef* anim) { m_attackAnim = anim; };

	// Input
	bool IsInputFrozen() const { return m_isFrozen; }
	void SetFreezeInput(bool isFreeze);
	void HandleJoystickInput(float deltaSeconds);

	// Skill
	void UseSkill(int skillID);

	// Mutators
	void GetDamage(int damage);
	void SetPos(Vec3 pos) { m_pos = Vec2(pos.x, pos.z); }
	void Translate(Vec2 trans) { m_pos += trans; }
	void Dash(float velocity, float time) {
		m_isDash = true; 
		m_isFrozen = true;
		m_dashCount = 0.f;
		m_dashMaxTime = time; 
		m_dashSpeed = velocity;
	}

	// Accessors
	Vec2 GetPos() const { return m_pos; }
	float GetForwardAngle() const { return m_forwardAngle; }
	int GetPlayerID() { return m_controllerID; }
	int GetCurrentHealth() { return m_curHealth; }
	bool IsDead() const { return m_isDead; }

public:
	// Attribute
	PlayerAttrib m_attribe;

private:
	// Status
	Vec2 m_pos = Vec2::ZERO;
	float m_forwardAngle = 0.f;
	float m_height = 0.f;

	VoxelMesh* m_bodyVoxel = nullptr;
	VoxelMesh* m_handVoxel = nullptr;
	GPUMesh* m_body = nullptr;
	GPUMesh* m_hand = nullptr;
	
	int m_controllerID = -1;
	Map* m_curMap = nullptr;
	Game* m_game = nullptr;

	VoxelAnimator* m_bodyAnimator = nullptr;
	VoxelAnimator* m_handAnimator = nullptr;
	VoxelAnimDef* m_damagedAnim = nullptr;
	VoxelAnimDef* m_attackAnim = nullptr;

	Rigidbody2D* m_rigidbody = nullptr;

	// Gameplay
	int m_curHealth = 100;
	bool m_isDead = false;
	bool m_isFrozen = false;
	SkillDefinition* m_skills[SKILL_NUM];
	float m_skillCoolDown[SKILL_NUM];
	bool m_isDash = false;
	float m_dashSpeed = 5.f;
	float m_dashCount = 0.f;
	float m_dashMaxTime = 0.f;
};