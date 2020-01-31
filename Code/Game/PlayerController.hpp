#pragma once
#include "Engine/Math/Vec2.hpp"

class VoxelMesh;
class GPUMesh;

struct PlayerAttrib
{
	int maxHealth;
	float movingSpeed = 10.f;
	int attack;
	int armor;
	int colliderSize;
}; 

class PlayerController
{
public:
	PlayerController(int controllerID) : m_controllerID(controllerID) {}
	~PlayerController();

	void BeginFrame();
	void Update(float deltaSeconds);
	void Render();

	void Initialize();
	void AddModel(std::string modelPath);

	// Input
	bool IsInputFrozen() const { return m_isFrozen; };
	void SetFreezeInput(bool isFreeze);
	void HandleJoystickInput(float deltaSeconds);

	// Gameplay
	void UseSkill(int skillID);

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
};