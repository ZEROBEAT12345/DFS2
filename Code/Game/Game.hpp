#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Audio/AudioSystem.hpp"
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

enum SoundType
{
	SOUND_TYPE_INVALID = -1,
	SOUND_TYPE_ATTRACT,
	SOUND_TYPE_MAINPLAY,
	SOUND_TYPE_PAUSE,
	SOUND_TYPE_PLAYER_SHOOT,
	SOUND_TYPE_PLAYER_HIT,
	SOUND_TYPE_PLAYER_DIE,
	NUM_SOUND_TYPE
};

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

	// Audio
	SoundID m_soundList[NUM_SOUND_TYPE];
	SoundPlaybackID m_soundPlaybackList[NUM_SOUND_TYPE];

	// Gameplay
	void WinGame(int playerID);
	bool IsWinning() { return m_isWinGame; }

	std::map<std::string, VoxelMesh*> m_voxelLib;

private:
	Camera* m_camera;
	Camera* m_HUDCamera;
	Pipeline* m_shader;
	Material* m_mat;

	// Developer
	bool m_lockCamera = false;
	bool m_isWinGame = false;
	int m_winPlayerID = -1;

	// Gameplay
	Map* m_curMap;
	
	std::map<std::string, PlayerAttrib*> m_playerInfo;
	std::map<std::string, ProjectileDef*> m_projectileInfo;
	std::map<std::string, SkillDefinition*> m_skillInfo;
	std::map<std::string, VoxelAnimDef*> m_animInfo;

	// 
	float m_gameTime = 100.f;
	float m_deathZoneStartTime = 50.f;

	// HUD
	GPUMesh* m_healthBarBg;
	GPUMesh* m_healthBarSlot;
	GPUMesh* m_healthBarBg_2;
	GPUMesh* m_healthBarSlot_2;
	GPUMesh* m_healthBar;
	GPUMesh* m_healthBar_2;
	GPUMesh* m_countDown;

	GPUMesh* m_skill1[MAX_PLAYER_NUM];
	GPUMesh* m_skill1CoolDown[MAX_PLAYER_NUM];
	GPUMesh* m_skill2[MAX_PLAYER_NUM];
	GPUMesh* m_skill2CoolDown[MAX_PLAYER_NUM];
	GPUMesh* m_skill3[MAX_PLAYER_NUM];
	GPUMesh* m_skill3CoolDown[MAX_PLAYER_NUM];
	GPUMesh* m_playerHeader[MAX_PLAYER_NUM];

	GPUMesh* m_winScreen = nullptr;
	GPUMesh* m_winPlayerHeader = nullptr;
	GPUMesh* m_winPlayerName = nullptr;
	GPUMesh* m_restartText = nullptr;

	// Test mesh
	VoxelMesh* m_charVoxel[4]; 

	// camera 
	Vec3 m_cameraPos = Vec3::ZERO;
	float cameraXangle = 65.f;
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