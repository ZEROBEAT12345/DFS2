#include "Engine/Render/ImmediateRenderer.hpp"
#include "Engine/Math/Algorithms/MathUtils.hpp"
#include "Engine/Math/Shapes/Disc.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Game/Map.hpp"
#include "Game/Projectile.hpp"
#include "Game/ProjectileDef.hpp"
#include "Game/PlayerController.hpp"

using namespace DebugRender;

extern ImmediateRenderer* g_theRenderer;

Map::~Map()
{
	for (int i = 0; i < MAX_PLAYER_NUM; i++)
	{
		delete m_players[i];
		m_players[i] = nullptr;
	}

}

void Map::Initialize()
{
	// Generate from xml file
}

void Map::Update(float deltaSeconds)
{
	// Handle Collision
	for(int i = 0; i < MAX_PLAYER_NUM; i++)
	{
		for (int j = 0; j < m_projectiles.size(); j++)
		{
			PlayerController* player = m_players[i];
			Projectile* p = m_projectiles[j];

			if (!p)
				continue;

			Disc discA = Disc(player->GetPos(), player->m_attribe.colliderSize);
			Disc discB = Disc(p->GetPos(), p->m_def->collisionRadius);

			if (p->m_isDead || player->IsDead())
			{
				continue;
			}

			if (p->GetPlayerID() == player->GetPlayerID())
			{
				continue;
			}

			if(DoDiscsOverlap(discA, discB))
			{
				int damage = p->m_def->damageCoef;
				player->GetDamage(damage);
				p->Die();

				DebugRenderMessage(3.f, Rgba::PINK_IKKONZOME, Rgba::PINK_IKKONZOME, "Bullet hit!!!");
			}
		}
	}

	for (int i = 0; i < MAX_PLAYER_NUM; i++)
	{
		m_players[i]->Update(deltaSeconds);
	}

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

	for (int i = 0; i < MAX_PLAYER_NUM; i++)
	{
		m_players[i]->Render();
	}

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