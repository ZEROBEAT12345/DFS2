#pragma once
#include <vector>

class Projectile;
class GPUMesh;

class Map
{
public:
	Map() {}
	~Map() {}
	void Update(float deltaSeconds);
	void Render();

	void DeleteGarbageEntities();

	void SetTerrain(GPUMesh* tMesh) { m_terrainMesh = tMesh; }
	void AddProjectile(Projectile* p);
	void RemoveProjectile(Projectile* p);

private:
	GPUMesh* m_terrainMesh;
	std::vector<Projectile*> m_projectiles;
};