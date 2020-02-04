#pragma once
#include "Engine/Math/Vec2.hpp"

class VoxelMesh;
class GPUMesh;
class Map;
class SkillDefinition;

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
	int colliderSize;
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
	void AddModel(std::string modelPath);
	void AddSkill(SkillDefinition* skill, int skillID) { m_skills[skillID] = skill; }

	// Input
	bool IsInputFrozen() const { return m_isFrozen; }
	void SetFreezeInput(bool isFreeze);
	void HandleJoystickInput(float deltaSeconds);

	// Skill
	void UseSkill(int skillID);

	Vec2 GetPos() const { return m_pos; }
	float GetForwardAngle() const { return m_forwardAngle; }

public:
	// Attribute
	PlayerAttrib m_attribe;

private:
	// Status
	Vec2 m_pos = Vec2(0.f,0.f);
	float m_forwardAngle = 0.f;
	float m_height = 0.f;

	VoxelMesh* m_voxel = nullptr;
	GPUMesh* m_mesh = nullptr;

	int m_curHealth = 0;
	bool m_isFrozen = false;
	
	int m_controllerID = -1;
	Map* m_curMap = nullptr;

	// Skill
	SkillDefinition* m_skills[SKILL_NUM];
};