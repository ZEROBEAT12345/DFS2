#pragma once
#include <vector>
#include <string>
#include "Engine/Math/IntVec2.hpp"
#include "Game/VoxelParticle.hpp"

class Projectile;
class VoxelMesh;
class CPUMesh;
class GPUMesh;
class PlayerController;

#define MAX_PLAYER_NUM 2
#define MAX_PARTICLE_NUM 1024

class Map
{
public:
	Map() {}
	~Map();
	void Initialize(std::string defFilePath);
	void Update(float deltaSeconds);
	void Render();

	void DeleteGarbageEntities();

	// Accessors
	int GetIndexFromTile(IntVec2 tile);
	IntVec2 GetTileFromIndex(int idx);
	PlayerController* PlayerA() const { return m_players[0]; }
	PlayerController* PlayerB() const { return m_players[1]; }
	IntVec2 GetMapGrid() const { return m_dimension; }
	Vec2 GetTileCenter(IntVec2 tile) 
	{
		return Vec2((tile.x + .5f) * m_gridScale, (tile.y + .5f) * m_gridScale);
	}

	Vec3 GetMapCenterWorld() const {
		return Vec3(m_dimension.x * m_gridScale / 2.f,
			0.f, m_dimension.y * m_gridScale / 2.f); }

	Vec3 GetPlayerStart(int playerID)
	{
		return Vec3(m_playerStart[playerID].x * m_gridScale,
			0.f, m_playerStart[playerID].y * m_gridScale);
	}

	// Collision
	void CheckPlayerSurroundedTiles(int playerID);

	// Mutators
	void GenerateTilesFromImages(std::string imageFile);
	void SetTerrain(GPUMesh* tMesh) { m_terrainMesh = tMesh; }
	void SetPlayer(int PlayerID, PlayerController* player) { m_players[PlayerID] = player; }
	void AddProjectile(Projectile* p);
	void RemoveProjectile(Projectile* p);

	// DeathZone
	void StartDeathZone() { m_isDeathZoneOn = true; }

	// Particle
	void spawnParticle(VoxelParticle prototype);

private:
	// Map info
	IntVec2 m_dimension;
	float m_gridScale = 1.f;
	std::vector<bool> m_collisionTile;
	IntVec2 m_playerStart[MAX_PLAYER_NUM];
	std::vector<IntVec2> m_ItemSpot;

	VoxelMesh* m_terrainVoxel = nullptr;
	VoxelMesh* m_borderVoxel = nullptr;
	GPUMesh* m_terrainMesh = nullptr;
	GPUMesh* m_borderMesh = nullptr;

	// Death Zone
	bool m_isDeathZoneOn = false;
	float m_deathZoneShrinkTime = 30.f;
	float m_deathZoneCurrentRatio = 1.f;
	int m_deathZoneDamage = 10;
	float m_deathZoneDamageCounter = 1.0f;
	GPUMesh* m_deathZoneMesh = nullptr;

	// Map stuffs
	// Particles
	float m_particleTimer = 0.f;
	VoxelParticle particlePrototype;
	VoxelParticle m_particles[MAX_PARTICLE_NUM];
	int m_curEmptyParticleSlot = 0;
	VoxelMesh* m_particleVoxels = nullptr;
	CPUMesh* m_particleCPUmesh = nullptr;
	GPUMesh* m_particleMesh = nullptr;
	PlayerController* m_players[MAX_PLAYER_NUM];
	std::vector<Projectile*> m_projectiles;
};