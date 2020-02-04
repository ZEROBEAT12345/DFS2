#include "Engine/Render/ImmediateRenderer.hpp"
#include "Game/Map.hpp"
#include "Game/Projectile.hpp"

extern ImmediateRenderer* g_theRenderer;

void Map::Update(float deltaSeconds)
{
	for(Projectile* p : m_projectiles)
	{
		if(!p || p->m_isDead)
			continue;

		p->Update(deltaSeconds);
	}
}

void Map::Render()
{
	// Render Terrain
	Matrix44 terrainModelMat = Matrix44::identity;
	terrainModelMat.SetTranslation(Vec3(0.f, -3.f, 12.f));
	g_theRenderer->BindModelMatrix(terrainModelMat);
	g_theRenderer->DrawMesh(m_terrainMesh);

	// Render items on map
	for(Projectile* p: m_projectiles)
	{
		if (!p)
			continue;

		p->Render();
	}
}

void Map::DeleteGarbageEntities()
{
	for (size_t i = 0; i < m_projectiles.size(); i++)
	{
		if (m_projectiles[i] && m_projectiles[i]->m_isDead)
		{
			Projectile* P = m_projectiles[i];
			RemoveProjectile(P);
			delete P;
		}
	}
}

void Map::AddProjectile(Projectile* p)
{
	bool isSpare = false;
	int spareNum;
	for (size_t i = 0; i < m_projectiles.size(); i++)
	{
		if (m_projectiles[i] == nullptr)
		{
			isSpare = true;
			spareNum = i;
			break;
		}
	}

	if (!isSpare)
		m_projectiles.push_back(p);
	else
		m_projectiles[spareNum] = p;
}

void Map::RemoveProjectile(Projectile* p)
{
	for (size_t i = 0; i < m_projectiles.size(); i++)
	{
		if (m_projectiles[i] == p)
		{
			m_projectiles[i] = nullptr;
			break;
		}
	}
}