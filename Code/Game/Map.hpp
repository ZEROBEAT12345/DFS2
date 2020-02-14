#pragma once
#include <vector>

class Projectile;
class VoxelMesh;
class GPUMesh;
class PlayerController;

#define MAX_PLAYER_NUM 2

class Map
{
public:
	Map() {}
	~Map();
	void Initialize();
	void Update(float deltaSeconds);
	void Render();

	void DeleteGarbageEntities();

	// Accessors
	PlayerController* PlayerA() const { return m_players[0]; }
	PlayerController* PlayerB() const { return m_players[1]; }

	// Mutators
	void SetTerrain(GPUMesh* tMesh) { m_terrainMesh = tMesh; }
	void SetPlayer(int PlayerID, PlayerController* player) { m_players[PlayerID] = player; }
	void AddProjectile(Projectile* p);
	void RemoveProjectile(Projectile* p);

private:
	VoxelMesh* m_terrainVoxel;
	GPUMesh* m_terrainMesh;
	PlayerController* m_players[MAX_PLAYER_NUM];
	std::vector<Projectile*> m_projectiles;
};