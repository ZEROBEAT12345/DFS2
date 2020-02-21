#include "Engine/Render/ImmediateRenderer.hpp"
#include "Engine/Math/Algorithms/MathUtils.hpp"
#include "Engine/Math/Shapes/Disc.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Game/Map.hpp"
#include "Game/Projectile.hpp"
#include "Game/ProjectileDef.hpp"
#include "Game/PlayerController.hpp"
#include "Game/VoxelMesh.hpp"

using namespace DebugRender;

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

		int tileNum = tileGrid.x * tileGrid.y;
		for (int i = 0; i < tileGrid.x; i++)
		{
			for (int j = 0; j < tileGrid.y; j++)
			{
				grid.pos = Vec3(i, j, -1);
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
				borderMat.SetTranslation(Vec3((i + .5f) * m_gridScale, 0.f, (j + .5f) * m_gridScale));
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