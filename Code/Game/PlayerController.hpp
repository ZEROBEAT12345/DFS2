#pragma once
#include "Engine/Math/Vec2.hpp"

class VoxelGrid;
class GPUMesh;

struct PlayerAttrib
{
	int maxHealth;
	float movingSpeed;
	int attack;
	int armor;
	int colliderSize;
}; 

class PlayerController
{
public:
	PlayerController() {}
	~PlayerController();

	void BeginFrame();
	void Update(float deltaSeconds);
	void Render();

	void Initialize();
	void AddModel(std::string modelPath);

	// Gameplay

public:
	// Attribute
	PlayerAttrib m_attribe;

private:
	// Status
	Vec2 m_pos = Vec2(0.f,0.f);
	float m_forwardAngle = 0.f;

	VoxelGrid* m_voxel;
	GPUMesh* m_mesh;

	int m_curHealth;
	
	int m_controllerIdx = 0;
};