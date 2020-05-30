#include "Engine/Render/ImmediateRenderer.hpp"
#include "Engine/Math/Algorithms/MathUtils.hpp"
#include "Engine/Math/Manifold2D.hpp"
#include "Engine/Math/Shapes/Disc.hpp"
#include "Engine/Math/Shapes/AABB2.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/Algorithms/RandomNumberGenerator.hpp"
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/Projectile.hpp"
#include "Game/ProjectileDef.hpp"
#include "Game/PlayerController.hpp"
#include "Game/VoxelMesh.hpp"

using namespace DebugRender;

extern RandomNumberGenerator* g_random;
extern ImmediateRenderer* g_theRenderer;

Map::~Map()
{
	for (int i = 0; i < MAX_PLAYER_NUM; i++)
	{
		delete m_players[i];
		m_players[i] = nullptr;
	}

	delete m_terrainMesh;
	m_terrainMesh = nullptr;

	delete m_terrainVoxel;
	m_terrainVoxel = nullptr;

	delete m_borderMesh;
	m_borderMesh = nullptr;

	delete m_borderVoxel;
	m_borderVoxel = nullptr;

	delete m_particleVoxels;
	m_particleVoxels = nullptr;

	delete m_particleCPUmesh;
	m_particleCPUmesh = nullptr;

	delete m_particleMesh;
	m_particleMesh = nullptr;
}

void Map::Initialize(std::string defFilePath)
{
	if (defFilePath == "")
		return;

	// Generate from xml file
	tinyxml2::XMLDocument mapDefXML;
	mapDefXML.LoadFile(defFilePath.c_str());
	XmlElement* mapRoot = mapDefXML.RootElement();

	//-----------------------------------------------------------------------------------------------
	// Terrain Step
	// Load terrain or generate voxel terrain 
	IntVec2 tileGrid = ParseXmlAttribute(*mapRoot, "grid", IntVec2(1, 1));
	float tileGridScale = ParseXmlAttribute(*mapRoot, "gridscale", 1.f);
	m_dimension = tileGrid;
	m_collisionTile = std::vector<bool>(m_dimension.x * m_dimension.y, false);
	m_gridScale = tileGridScale;

	std::string terrainMeshPath = ParseXmlAttribute(*mapRoot, "id", "");
	std::string tileImagePath = ParseXmlAttribute(*mapRoot, "tileImg", "");
	Rgba tileColor = ParseXmlAttribute(*mapRoot, "color", Rgba::WHITE);
	if(terrainMeshPath != "") // Load terrain voxel
	{
		m_terrainVoxel = new VoxelMesh();
		m_terrainVoxel->LoadFromFiles(terrainMeshPath);

	}
	else // Generate terrain voxel
	{
		m_terrainVoxel = new VoxelMesh();
		VoxelGrid grid = {};
		grid.Color = tileColor;

		for (int i = 0; i < tileGrid.x; i++)
		{
			for (int j = 0; j < tileGrid.y; j++)
			{
				grid.pos = Vec3((float)i, (float)j, 0);
				m_terrainVoxel->AddVoxel(grid);
			}
		}
	}

	// Generate mesh from voxels
	m_terrainMesh = new GPUMesh(g_theRenderer->GetCTX());
	CPUMesh* terrainMesh = m_terrainVoxel->GenerateMesh(tileGridScale);
	m_terrainMesh->CreateFromCPUMesh(terrainMesh, VERTEX_TYPE_LIGHT);
	delete terrainMesh;

	// Generate tiles using image
	GenerateTilesFromImages(tileImagePath);

	//-----------------------------------------------------------------------------------------------
	// Stuff on map

	XmlElement* parameterDef = mapRoot->FirstChildElement();

	while(parameterDef)
	{
		std::string paraType = parameterDef->Name();

		if( paraType == "healthBall" )
		{
			// Initialize health ball on the map
			// TBD
			break;
		}
		
		if (paraType == "border")
		{
			std::string borderPath = ParseXmlAttribute(*parameterDef, "mesh", "");
			if (borderPath == "")
				break;

			float borderScale = ParseXmlAttribute(*parameterDef, "scale", 1.f);

			m_borderVoxel = new VoxelMesh();
			m_borderVoxel->LoadFromFiles(borderPath);

			m_borderMesh = new GPUMesh(g_theRenderer->GetCTX());
			CPUMesh* borderMesh = m_borderVoxel->GenerateMesh(borderScale);
			m_borderMesh->CreateFromCPUMesh(borderMesh, VERTEX_TYPE_LIGHT);
			delete borderMesh;
		}

		parameterDef = parameterDef->NextSiblingElement();
	}

	// Prototype particle
	particlePrototype = VoxelParticle
	{
		false,
		nullptr,
		nullptr,
		Vec3::ZERO,
		Vec3(0.f, 0.f, 5.f),
		{
			Rgba::RED,
			Rgba::PINK_TAIKOH,
			Rgba::YELLOW
		},
		0.5f,
		0.f,
		10.f,
		{0.f,0.f,0.f},
		{0.f,0.f,0.f},
		{0.f,0.f,0.f},
		m_theGame->m_voxelLib["Apple"]
	};

	for(int i= 0; i < MAX_PARTICLE_NUM; i++)
	{
		m_particles[i] = VoxelParticle();
		//m_particles[i].AddMesh(m_theGame->m_voxelLib["Apple"]);
	}
}

void Map::Update(float deltaSeconds)
{
	// Handle Collision For Player and bullets
	for(int i = 0; i < MAX_PLAYER_NUM; i++)
	{
		for (int j = 0; j < m_projectiles.size(); j++)
		{
			PlayerController* player = m_players[i];
			Projectile* p = m_projectiles[j];

			if (!p || player->IsDead())
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
				p->OnOverlapWithPlayer(player);
				//int damage = (int)p->m_def->damageCoef;
				//player->GetDamage(damage);
				//p->Die();

				DebugRenderMessage(3.f, Rgba::PINK_IKKONZOME, Rgba::PINK_IKKONZOME, "Bullet hit!!!");
			}
		}
	}

	// Handled Collision For players
	Disc discA = Disc(m_players[0]->GetPos(), m_players[0]->m_attribe.colliderSize);
	Disc discB = Disc(m_players[1]->GetPos(), m_players[1]->m_attribe.colliderSize);
	Manifold2D* m = new Manifold2D();
	if(GetManifoldForDiscWithDisc(m, discA, discB))
	{
		m_players[0]->m_pos += m->normal * m->penetration / 2.f;
		m_players[1]->m_pos -= m->normal * m->penetration / 2.f;
	}

	// Handled Collision For tiles and players
	CheckPlayerSurroundedTiles(0);
	CheckPlayerSurroundedTiles(1);

	// Handled Collision For projectiles and tiles
	for (Projectile* p : m_projectiles)
	{
		if (!p || p->m_isDead)
			continue;

		CheckProjectileSurroundedTiles(p);
	}

	for (int i = 0; i < MAX_PLAYER_NUM; i++)
	{
		if(!m_players[i]->IsDead())
		m_players[i]->Update(deltaSeconds);
	}

	for(Projectile* p : m_projectiles)
	{
		if(!p || p->m_isDead)
			continue;

		p->Update(deltaSeconds);
	}

	// Update death zone
	if(m_isDeathZoneOn)
	{
		if(m_deathZoneCurrentRatio > 0.f)
			m_deathZoneCurrentRatio -= deltaSeconds / m_deathZoneShrinkTime;

		m_deathZoneDamageCounter += deltaSeconds;
		if(m_deathZoneDamageCounter >= 1.0f)
		{
			m_deathZoneDamageCounter -= 1.0f;

			// Deal Damage
			Vec3 mapCenterWorld = GetMapCenterWorld();
			DealAOEDamage(Vec2(mapCenterWorld.x, mapCenterWorld.z), m_dimension.x * m_gridScale / 2.f * m_deathZoneCurrentRatio, m_deathZoneDamage, false);

			/*Disc deathZoneDisc = Disc(Vec2(mapCenterWorld.x, mapCenterWorld.z), m_dimension.x * m_gridScale / 2.f * m_deathZoneCurrentRatio);

			for(int i = 0; i < MAX_PLAYER_NUM; i++)
			{
				Disc playerDisc = Disc(m_players[i]->m_pos, m_players[i]->m_attribe.colliderSize);
				if(!DoDiscsOverlap(deathZoneDisc, playerDisc))
				{
					m_players[i]->GetDamage(m_deathZoneDamage);
				}
			}*/
		}
	}
	
	// Particle
	int randomAngle;
	m_particleTimer += deltaSeconds;
	if(m_particleTimer > .5f)
	{
		m_particleTimer -= .5f;
		for (int i = 0; i < 10; i++)
		{
			randomAngle = g_random->GetRandomIntInRange(0, 360);
			particlePrototype.velocity = Vec3(CosDegrees((float)randomAngle), SinDegrees((float)randomAngle), 1.f);
			spawnParticle(particlePrototype);
		}
	}

	if (m_particleVoxels)
		delete m_particleVoxels;

	m_particleVoxels = new VoxelMesh();

	for(int i = 0; i < MAX_PARTICLE_NUM; i++)
	{
		if(m_particles[i].isAlive)
		{
			m_particles[i].Update(deltaSeconds);

			if(m_particles[i].replaceMesh)
			{
				Matrix44 newTrans = Matrix44::MakeTranslation3D(m_particles[i].pos);
				m_particleVoxels->MergeVoxelMesh(m_particles[i].replaceMesh, newTrans);
			}
			else
			{
				VoxelGrid voxel =
				{
					m_particles[i].pos,
					m_particles[i].GetcurColor()
				};
				m_particleVoxels->AddVoxel(voxel);
			}

		}	
	}

	if (m_particleCPUmesh)
		delete m_particleCPUmesh;

	m_particleCPUmesh = m_particleVoxels->GenerateMesh(2.f);

	if (m_particleMesh)
		delete m_particleMesh;

	m_particleMesh = new GPUMesh(g_theRenderer->GetCTX());
	if(!m_particleCPUmesh->IsEmpty())
		m_particleMesh->CreateFromCPUMesh(m_particleCPUmesh, VERTEX_TYPE_LIGHT);
}

void Map::Render()
{
	// Render Terrain
	Matrix44 terrainModelMat = Matrix44::identity;
	terrainModelMat.SetTranslation(Vec3(0.f, (-.5f) * m_gridScale, 0.f));
	g_theRenderer->BindModelMatrix(terrainModelMat);
	g_theRenderer->DrawMesh(m_terrainMesh);

	for (int i = 0; i < MAX_PLAYER_NUM; i++)
	{
		if(!m_players[i]->IsDead())
		m_players[i]->Render();
	}

	// Render stuffs on map
	for (int i = 0; i < m_dimension.x; i++)
	{
		for(int j = 0; j < m_dimension.y; j++)
		{
			int idx = GetIndexFromTile(IntVec2(i, j));
			if(m_collisionTile[idx])
			{
				// Render borders
				Matrix44 borderMat = Matrix44::identity;
				borderMat.SetTranslation(Vec3(i * m_gridScale, 0.f, j * m_gridScale));
				g_theRenderer->BindModelMatrix(borderMat);
				g_theRenderer->DrawMesh(m_borderMesh);
			}
		}
	}

	// Render items on map
	for(Projectile* p: m_projectiles)
	{
		if (!p)
			continue;

		p->Render();
	}

	// Render death zone
	if(m_isDeathZoneOn)
	{
		GPUMesh* dzMesh = new GPUMesh(g_theRenderer->GetCTX());
		CPUMesh* deathZoneMesh = new CPUMesh();
		CPUMeshAddDisc2D(deathZoneMesh, Vec2::ZERO, m_deathZoneCurrentRatio * m_gridScale * m_dimension.x / 2.f, Rgba(.8f, 0.f, 0.f, .5f), 20);
		dzMesh->CreateFromCPUMesh(deathZoneMesh, VERTEX_TYPE_LIGHT);

		Matrix44 deathZoneMat = Matrix44::MakeXRotationDegrees(90.f);
		Matrix44 translation = Matrix44::MakeTranslation3D(Vec3(m_gridScale * m_dimension.x / 2.f, .5f, m_gridScale * m_dimension.y / 2.f));

		g_theRenderer->BindModelMatrix(translation * deathZoneMat);
		g_theRenderer->DrawMesh(dzMesh);

		delete dzMesh;
		delete deathZoneMesh;
	}

	// Render particles
	g_theRenderer->BindModelMatrix(Matrix44::MakeTranslation3D(GetMapCenterWorld()));
	g_theRenderer->DrawMesh(m_particleMesh);
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

int Map::GetIndexFromTile(IntVec2 tile)
{
	return tile.x + m_dimension.x * tile.y;
}

IntVec2 Map::GetTileFromIndex(int idx)
{
	int x = idx % m_dimension.x;
	int y = idx / m_dimension.x;

	return IntVec2(x, y);
}

float Map::GetPlayerCooldown(int playerID, int skillID) 
{ 
	return m_players[playerID]->m_skillCoolDown[skillID]; 
}

void Map::CheckPlayerSurroundedTiles(int playerID)
{
	// Only check surrounded tiles for player
	int x = (int)floor(m_players[playerID]->m_pos.x / m_gridScale + 0.5f);
	int y = (int)floor(m_players[playerID]->m_pos.y / m_gridScale + 0.5f);
	int curX = x;
	int curY = y;
	int dx = 1;
	int dy = 1;

	int tileRangeA = (int)floor(m_players[playerID]->m_attribe.colliderSize / m_gridScale) + 1;
	for (int r = 0; r <= tileRangeA; r++)
	{
		curX = 0 + x;
		curY = r + y;
		dx = 1;
		dy = 1;

		for (int i = 0; i < 4 * r; i++)
		{
			if (curX - x == 0 || curY - y == 0)
			{
				if (dx == 1 && dy == -1)
				{
					dx = -1;
					dy = -1;
				}
				else if (dx == -1 && dy == -1)
				{
					dx = -1;
					dy = 1;
				}
				else if (dx == -1 && dy == 1)
				{
					dx = 1;
					dy = 1;
				}
				else if (dx == 1 && dy == 1)
				{
					dx = 1;
					dy = -1;
				}
			}

			// Resolve Collision for each tile
			if (curX >= 0 && curX < m_dimension.x && curY >= 0 && curY < m_dimension.y)
			{
				int idx = GetIndexFromTile(IntVec2(curX, curY));
				if (m_collisionTile[idx])
				{
					Vec2 bottomLeft = Vec2(curX * m_gridScale, curY * m_gridScale) - Vec2(m_gridScale, m_gridScale) * .5f;
					Vec2 topRight = bottomLeft + Vec2(m_gridScale, m_gridScale);
					AABB2 tileBox = AABB2(bottomLeft, topRight);
					Disc disc = Disc(m_players[playerID]->GetPos(), m_players[playerID]->m_attribe.colliderSize);

					Manifold2D* m = new Manifold2D();
					if (GetManifoldForAABB2WithDisc(m, tileBox, disc))
					{
						m_players[playerID]->m_pos -= m->normal * m->penetration;
					}
				}

			}

			curX += dx;
			curY += dy;
		}
	}
}

void Map::CheckProjectileSurroundedTiles(Projectile* p)
{
	// Only check surrounded tiles for player
	int x = (int)floor(p->m_pos.x / m_gridScale + 0.5f);
	int y = (int)floor(p->m_pos.y / m_gridScale + 0.5f);
	int curX = x;
	int curY = y;
	int dx = 1;
	int dy = 1;

	int tileRangeA = (int)floor(p->m_def->collisionRadius / m_gridScale) + 1;
	for (int r = 0; r <= tileRangeA; r++)
	{
		curX = 0 + x;
		curY = r + y;
		dx = 1;
		dy = 1;

		for (int i = 0; i < 4 * r; i++)
		{
			if (curX - x == 0 || curY - y == 0)
			{
				if (dx == 1 && dy == -1)
				{
					dx = -1;
					dy = -1;
				}
				else if (dx == -1 && dy == -1)
				{
					dx = -1;
					dy = 1;
				}
				else if (dx == -1 && dy == 1)
				{
					dx = 1;
					dy = 1;
				}
				else if (dx == 1 && dy == 1)
				{
					dx = 1;
					dy = -1;
				}
			}

			// Resolve Collision for each tile
			if (curX >= 0 && curX < m_dimension.x && curY >= 0 && curY < m_dimension.y)
			{
				int idx = GetIndexFromTile(IntVec2(curX, curY));
				if (m_collisionTile[idx])
				{
					Vec2 bottomLeft = Vec2(curX * m_gridScale, curY * m_gridScale) - Vec2(m_gridScale, m_gridScale) * .5f;
					Vec2 topRight = bottomLeft + Vec2(m_gridScale, m_gridScale);
					AABB2 tileBox = AABB2(bottomLeft, topRight);
					Disc disc = Disc(p->m_pos, p->m_def->collisionRadius);

					Manifold2D* m = new Manifold2D();
					if (GetManifoldForAABB2WithDisc(m, tileBox, disc))
					{
						p->OnHit();
					}
				}

			}

			curX += dx;
			curY += dy;
		}
	}
}

void Map::GenerateTilesFromImages(std::string imageFile)
{
	Image tileImg = Image(imageFile.c_str());

	// Go through all pixels generating tiles
	for(int i = 0; i < m_dimension.x; i++)
	{
		for(int j = 0; j < m_dimension.y; j++)
		{
			Rgba texelData = tileImg.GetTexelColor(IntVec2(i, j));

			// Plain tile
			if(texelData == Rgba::WHITE)
				continue;

			// Border tile
			if(texelData == Rgba::BLACK)
			{
				int idx = GetIndexFromTile(IntVec2(i, j));
				m_collisionTile[idx] = true;
			}

			// Player Start
			if (texelData == Rgba::BLUE)
			{
				m_playerStart[0] = IntVec2(i, j);
			}

			if (texelData == Rgba::GREEN)
			{
				m_playerStart[1] = IntVec2(i, j);
			}

			// Item pas
			if (texelData == Rgba::RED)
			{
				m_ItemSpot.push_back(IntVec2(i, j));
			}
		}
	}
}

void Map::AddProjectile(Projectile* p)
{
	bool isSpare = false;
	size_t spareNum = 0;
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

void Map::spawnParticle(VoxelParticle prototype)
{
	for(int i = m_curEmptyParticleSlot; i < MAX_PARTICLE_NUM; i++)
	{
		if(m_particles[i].isAlive == false)
		{
			m_particles[i] = prototype;
			m_particles[i].isAlive = true;
			m_curEmptyParticleSlot = i;
			return;
		}
	}

	for (int i = 0; i < m_curEmptyParticleSlot; i++)
	{
		if (m_particles[i].isAlive == false)
		{
			m_particles[i] = prototype;
			m_particles[i].isAlive = true;
			m_curEmptyParticleSlot = i;
			return;
		}
	}
}

void Map::DealAOEDamage(Vec2 pos, float radius, int damage, bool inSide)
{
	// Deal Damage
	Disc disc = Disc(pos, radius);

	for (int i = 0; i < MAX_PLAYER_NUM; i++)
	{
		Disc playerDisc = Disc(m_players[i]->m_pos, m_players[i]->m_attribe.colliderSize);
		if (DoDiscsOverlap(disc, playerDisc))
		{
			if(inSide)
				m_players[i]->GetDamage(damage);
		}
		else
		{
			if(!inSide)
				m_players[i]->GetDamage(damage);
		}
	}
}