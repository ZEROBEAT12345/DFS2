#pragma once
#include "Engine/Math/Vec2.hpp"

class VoxelMesh;
class GPUMesh;
class Map;
class SkillDefinition;
class VoxelAnimator;
class VoxelAnimDef;

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
	int maxHealth;
	float movingSpeed = 10.f;
	int attack;
	int armor;
	int colliderSize = 10.f;
	std::string SkillID_1;
	int SkillID_2;
	int SkillID_3;
	int SkillID_4;
}; 

class PlayerController
{
	friend SkillDefinition;

public:
	PlayerController(int controllerID, Map* map) : m_controllerID(controllerID), m_curMap(map) {}
	~PlayerController();

	void BeginFrame();
	void Update(float deltaSeconds);
	void Render();

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

	// Accessors
	Vec2 GetPos() const { return m_pos; }
	float GetForwardAngle() const { return m_forwardAngle; }
	int GetPlayerID() { return m_controllerID; }
	bool IsDead() const { return m_isDead; }

public:
	// Attribute
	PlayerAttrib m_attribe;

private:
	// Status
	Vec2 m_pos = Vec2(0.f,0.f);
	float m_forwardAngle = 0.f;
	float m_height = 0.f;

	VoxelMesh* m_bodyVoxel = nullptr;
	VoxelMesh* m_handVoxel = nullptr;
	GPUMesh* m_body = nullptr;
	GPUMesh* m_hand = nullptr;
	
	int m_controllerID = -1;
	Map* m_curMap = nullptr;

	VoxelAnimator* m_bodyAnimator = nullptr;
	VoxelAnimator* m_handAnimator = nullptr;
	VoxelAnimDef* m_damagedAnim = nullptr;
	VoxelAnimDef* m_attackAnim = nullptr;

	// Gameplay
	int m_curHealth = 100;
	bool m_isDead = false;
	bool m_isFrozen = false;
	SkillDefinition* m_skills[SKILL_NUM];
};