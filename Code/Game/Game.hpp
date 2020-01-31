#pragma once
#include "Engine/Math/Vec3.hpp"

struct Camera;
class Pipeline;
class GPUMesh;
class Material;
class Scene;
class Model;
class SkyBox;
class Cubemap;
class VoxelMesh;
class PlayerController;

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

	// Camera

	// Light
	void AdjustAmbient(float deltaTime);
	void SetAmbientChangeAmount(float amount);

private:
	Camera* m_camera;
	Pipeline* m_shader;
	Material* m_mat;

	PlayerController* m_players[MAX_PLAYER_NUM];

	// Test mesh
	GPUMesh* m_cube;
	GPUMesh* m_sphere;
	GPUMesh* m_quad;
	VoxelMesh* m_charVoxel[4];
	VoxelMesh* m_terrainVoxel;
	GPUMesh* m_vMesh[4];
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