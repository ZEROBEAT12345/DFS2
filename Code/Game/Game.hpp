#pragma once
#include "Engine/Math/Vec3.hpp"
#include <map>

struct Camera;
class Pipeline;
class GPUMesh;
class Material;
class Scene;
class Model;
class SkyBox;
class Cubemap;
class VoxelMesh;
class Map;
class PlayerController;
class ProjectileDef;
class SkillDefinition;
struct PlayerAttrib;
class VoxelAnimDef;

#define MAX_PLAYER_NUM 2

class Game
{
public:
	Game();
	~Game();
	void Startup();
	void Shutdown();
	void Update(float deltaSeconds);
	void Render();

	void DeleteGarbageEntities();
	void HandleKeyPressed(unsigned char KeyCode);
	void HandleKeyReleased(unsigned char KeyCode);

	// Setup
	void LoadResources();

	// Light
	void AdjustAmbient(float deltaTime);
	void SetAmbientChangeAmount(float amount);

private:
	Camera* m_camera;
	Pipeline* m_shader;
	Material* m_mat;

	// Gameplay
	Map* m_curMap;
	std::map<std::string, PlayerAttrib*> m_playerInfo;
	std::map<std::string, ProjectileDef*> m_projectileInfo;
	std::map<std::string, SkillDefinition*> m_skillInfo;
	std::map<std::string, VoxelAnimDef*> m_animInfo;

	// Test mesh
	GPUMesh* m_cube;
	GPUMesh* m_sphere;
	GPUMesh* m_quad;
	VoxelMesh* m_charVoxel[4];
	VoxelMesh* m_terrainVoxel;
	GPUMesh* m_tMesh;

	// camera 
	Vec3 m_cameraPos = Vec3::ZERO;
	float cameraXangle = 50.f;
	float cameraYangle = 0.f;
	Vec3 cameraRotateSpeed;
	float cameraMovingSpeed;
	Vec3 cameraMovingDir;
	bool cameraMoveFront = false;
	bool cameraMoveBack = false;
	bool cameraMoveLeft = false;
	bool cameraMoveRight = false;

	// light
	float xzAngle = 0.f;
	float ambientChangeAmount = 0.f;
	float ambientLightVolume = 1.f;
};