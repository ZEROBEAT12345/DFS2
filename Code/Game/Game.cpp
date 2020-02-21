#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Debug/MemoryTrack.hpp"
#include "Engine/Core/Debug/Log.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/Algorithms/RandomNumberGenerator.hpp"
#include "Engine/Math/Algorithms/MathUtils.hpp"
#include "Engine/Math/Shapes/AABB2.hpp"
#include "Engine/Render/ImmediateRenderer.hpp"
#include "Engine/Render/AssetLoader.hpp"
#include "Engine/Render/Frame/RHIDeviceContext.hpp"
#include "Engine/Render/Utils/VertexUtils.hpp"
#include "Engine/Render/Utils/SpriteSheet.hpp"
#include "Engine/Render/Utils/BitmapFont.hpp"
#include "Engine/Render/Utils/Camera.hpp"
#include "Game/Game.hpp"
#include "GameCommon.hpp"
#include "Game/VoxelMesh.hpp"
#include "Game/PlayerController.hpp"
#include "Game/SkillDefinition.hpp"
#include "Game/ProjectileDef.hpp"
#include "Game/Projectile.hpp"
#include "Game/Map.hpp"
#include "Game/VoxelAnimDef.hpp"
#include "Game/VoxelAnimator.hpp"
//#include "Engine/Tools/FBXImporter.hpp"
#include <vector>

extern ImmediateRenderer* g_theRenderer;
extern AssetLoader* g_assetLoader;
extern AudioSystem* g_theAudio;
extern DevConsole* g_theDevConsole;
extern BitmapFont* g_testFont;
extern EventSystem* g_theEventSystem;
extern WindowContext* g_theWindow;
using namespace DebugRender;

struct ProjectorBufferT
{
	Matrix44 view;
	Matrix44 projection;
	Vec4 projectPos;
};

struct TonemapBufferT
{
	Matrix44 transform;

	float strength;
	Vec3 pad00;
};

bool TestEvent(EventArgs& TEST)
{
	UNUSED(TEST);
	g_theDevConsole->PrintString("TestEvent1", Rgba::RED);
	return false;
}

bool TestEvent2(EventArgs& TEST)
{
	g_theDevConsole->PrintString("TestEvent2", Rgba::RED);
	g_theDevConsole->PrintString(TEST.GetValue("first", "hello"), Rgba::RED);
	g_theDevConsole->PrintString(TEST.GetValue("second", "hello"), Rgba::RED);
	return false;
}

bool TurnOnDebugRender(EventArgs& TEST)
{
	UNUSED(TEST);
	DebugRenderShow(true);
	return false;
}

bool TurnOffDebugRender(EventArgs& TEST)
{
	UNUSED(TEST);
	DebugRenderShow(false);
	return false;
}

bool ClearDebugRender(EventArgs& TEST)
{
	UNUSED(TEST);
	DebugRenderClear();
	return false;
}

bool SetDirLightColor(EventArgs& Color)
{
	Rgba color = Color.GetValue("color", Rgba::WHITE);
	g_theRenderer->SetDirLightColor(color);
	DebugRenderMessage(3.f, Rgba::WHITE, Rgba::WHITE, "DirColor: (%f, %f, %f, %f)", color.r,color.g,color.b,color.a);
	return false;
}

bool SetDirLightDir(EventArgs& Dir)
{
	Vec3 dir = Dir.GetValue("dir", Vec3::ONE);
	g_theRenderer->SetDirLightDir(dir);
	DebugRenderMessage(3.f, Rgba::WHITE, Rgba::WHITE, "DirDir: (%f, %f, %f", dir.x, dir.y, dir.z);
	return false;
}

bool TrackMemoryCount(EventArgs& arg)
{
	size_t count = MemTrackGetLiveAllocationCount();

	g_theDevConsole->PrintString("Alloc Count:" + std::to_string(count), Rgba(0.9f, 0.5f, 0.5f, 1.0f));
	return false;
}

bool TrackMemoryByteCount(EventArgs& arg)
{
	size_t byteCount = MemTrackGetLiveByteCount();

	g_theDevConsole->PrintString("Byte Count:" + GetSizeString(byteCount), Rgba(0.9f, 0.5f, 0.5f, 1.0f));
	return false;
}

bool LogTrackMemory(EventArgs& arg)
{
	MemTrackLogLiveAllocations();
	return false;
}

bool AddFilter(EventArgs& arg)
{
	std::string filter = arg.GetValue("filter", "Warning");
	AddLogFilter(filter);
	return false;
}

bool TestLog(EventArgs& arg)
{
	std::string log = arg.GetValue("text", "TESTLOG");
	std::string filter = arg.GetValue("filter", "WARNING");
	Logf(filter.c_str(), "%s", log.c_str());
	return false;
}

Game::Game()
{

}

Game::~Game()
{

}

void Game::Startup()
{
	// Initialize 
	m_mat = g_assetLoader->CreateOrGetMaterialFromXMLFile("Data/Materials/voxel.mat");

	// Bind event functions

	g_theEventSystem->SubscribeEventCallbackFunction("DebugOn", TurnOnDebugRender);
	g_theEventSystem->SubscribeEventCallbackFunction("DebugOff", TurnOffDebugRender);
	g_theEventSystem->SubscribeEventCallbackFunction("DebugClear", ClearDebugRender);
	g_theEventSystem->SubscribeEventCallbackFunction("DirColor", SetDirLightColor);
	g_theEventSystem->SubscribeEventCallbackFunction("DirDir", SetDirLightDir);

	// Initialize test scene

	VoxelMesh* appleVoxel = new VoxelMesh();
	appleVoxel->LoadFromFiles("Data/Models/Ply/apple.ply");
	GPUMesh* aMesh = new GPUMesh(g_theRenderer->GetCTX());
	CPUMesh* appleMesh = appleVoxel->GenerateMesh(0.5f);
	aMesh->CreateFromCPUMesh(appleMesh, VERTEX_TYPE_LIGHT);
	g_assetLoader->RegisterMesh("Apple", aMesh);

	LoadResources();

	// Initialize Map
	m_curMap = new Map();
	m_curMap->Initialize("Data/Gameplay/Map/grassland.map");

	// Initialize Player
	for(int i = 0; i < MAX_PLAYER_NUM; i++)
	{
		PlayerController* newPlayer;
		newPlayer = new PlayerController(i, m_curMap);
		newPlayer->Initialize();
		newPlayer->AddModel("Data/Models/Ply/Newton.ply", "Data/Models/Ply/hand_Test.ply");
		newPlayer->AddSkill(m_skillInfo["Newton_0"], SKILL_NORMAL_ATTACK);
		newPlayer->AddDamagedAnim(m_animInfo["test"]);
		newPlayer->AddAttackAnim(m_animInfo["test"]);
		newPlayer->SetPos(m_curMap->GetPlayerStart(i));
		m_curMap->SetPlayer(i, newPlayer);
	}

	// Initialize camera config
	cameraMovingDir = Vec3::ZERO;
	cameraMovingSpeed = 10.0f;

	TextureView* rtv = g_theRenderer->GetDefaultRTV();
	m_camera = new Camera();
	m_camera->SetPerspectiveProjection(90.f, windowAspect, 0.1f, 1000.f);
	m_camera->SetColorTargetView(rtv);
	m_cameraPos = Vec3(0.f, 110.f, -30.f) + m_curMap->GetMapCenterWorld();

	Matrix44 cameraMat = Matrix44::MakeRotationForEulerZXY(Vec3(cameraXangle, cameraYangle, 0.f), m_cameraPos);
	m_camera->SetModelMatrix(cameraMat);

	m_HUDCamera = new Camera();
	m_HUDCamera->SetOrthographicProjection(Vec2::ZERO, Vec2(150.f, 100.f));
	m_HUDCamera->SetColorTargetView(rtv);

	// Initialize light config
	g_theRenderer->SetAmbientLight(1.0);

	// Initialize HUD
	CPUMesh healthBarBg = CPUMesh();
	AABB2 bgBox = AABB2(Vec2(2.f, 93.f), Vec2(44.f, 100.f));
	CPUMeshAddBox2D(&healthBarBg, bgBox, Rgba::WHITE, Vec2::ZERO, Vec2::ONE);
	m_healthBarBg = new GPUMesh(g_theRenderer->GetCTX());
	m_healthBarBg->CreateFromCPUMesh(&healthBarBg, VERTEX_TYPE_LIGHT);

	CPUMesh healthBarBg2 = CPUMesh();
	AABB2 bgBox2 = AABB2(Vec2(106.f, 93.f), Vec2(148.f, 100.f));
	CPUMeshAddBox2D(&healthBarBg2, bgBox2, Rgba::WHITE, Vec2::ZERO, Vec2::ONE);
	m_healthBarBg_2 = new GPUMesh(g_theRenderer->GetCTX());
	m_healthBarBg_2->CreateFromCPUMesh(&healthBarBg2, VERTEX_TYPE_LIGHT);

	CPUMesh healthBarSlot = CPUMesh();
	AABB2 slotBox = AABB2(Vec2(3.f, 94.f), Vec2(43.f, 99.f));
	CPUMeshAddBox2D(&healthBarSlot, slotBox, Rgba::BLUE_KON, Vec2::ZERO, Vec2::ONE);
	m_healthBarSlot = new GPUMesh(g_theRenderer->GetCTX());
	m_healthBarSlot->CreateFromCPUMesh(&healthBarSlot, VERTEX_TYPE_LIGHT);

	CPUMesh healthBarSlot_2 = CPUMesh();
	AABB2 slotBox2 = AABB2(Vec2(107.f, 94.f), Vec2(147.f, 99.f));
	CPUMeshAddBox2D(&healthBarSlot_2, slotBox2, Rgba::BLUE_KON, Vec2::ZERO, Vec2::ONE);
	m_healthBarSlot_2 = new GPUMesh(g_theRenderer->GetCTX());
	m_healthBarSlot_2->CreateFromCPUMesh(&healthBarSlot_2, VERTEX_TYPE_LIGHT);

	CPUMesh healthBar = CPUMesh();
	AABB2 barBox = AABB2(Vec2(3.f, 94.f), Vec2(43.f, 99.f));
	CPUMeshAddBox2D(&healthBar, barBox, Rgba::PINK_TAIKOH, Vec2::ZERO, Vec2::ONE);
	m_healthBar = new GPUMesh(g_theRenderer->GetCTX());
	m_healthBar->CreateFromCPUMesh(&healthBar, VERTEX_TYPE_LIGHT);

	CPUMesh healthBar2 = CPUMesh();
	AABB2 barBox2 = AABB2(Vec2(107.f, 94.f), Vec2(147.f, 99.f));
	CPUMeshAddBox2D(&healthBar2, barBox2, Rgba::PINK_TAIKOH, Vec2::ZERO, Vec2::ONE);
	m_healthBar_2 = new GPUMesh(g_theRenderer->GetCTX());
	m_healthBar_2->CreateFromCPUMesh(&healthBar2, VERTEX_TYPE_LIGHT);
}

void Game::Shutdown()
{
	delete m_curMap;
	m_curMap = nullptr;

	delete m_camera;
	m_camera = nullptr;

	delete m_HUDCamera;
	m_HUDCamera = nullptr;

	delete m_healthBarBg;
	m_healthBarBg = nullptr;

	delete m_healthBarBg_2;
	m_healthBarBg_2 = nullptr;

	delete m_healthBarSlot;
	m_healthBarSlot = nullptr;

	delete m_healthBarSlot_2;
	m_healthBarSlot_2 = nullptr;

	delete m_healthBar;
	m_healthBar = nullptr;

	delete m_healthBar_2;
	m_healthBar_2 = nullptr;
}

void Game::Update(float deltaSeconds)
{
	// Update camera config
	if(!m_lockCamera)
	{
		cameraMovingDir = Vec3::ZERO;
		if (cameraMoveFront)
			cameraMovingDir += Vec3(0.f, 0.f, 1.f);

		if (cameraMoveBack)
			cameraMovingDir += Vec3(0.f, 0.f, -1.f);

		if (cameraMoveLeft)
			cameraMovingDir += Vec3(-1.f, 0.f, 0.f);

		if (cameraMoveRight)
			cameraMovingDir += Vec3(1.f, 0.f, 0.f);

		Matrix44 cameraRotateMat = Matrix44::MakeRotationForEulerZXY(Vec3(cameraXangle, cameraYangle, 0.f), Vec3::ZERO);
		Vec3 cameraLocalTrans = cameraMovingDir * cameraMovingSpeed * deltaSeconds;
		Vec4 cameraWorldTrans = cameraRotateMat * Vec4(cameraLocalTrans.x, cameraLocalTrans.y, cameraLocalTrans.z, 0.f);
		m_cameraPos += Vec3(cameraWorldTrans.x, cameraWorldTrans.y, cameraWorldTrans.z);


		IntVec2 mouseTrans = g_theWindow->GetClientMouseRelativePositon();
		float Yangle = mouseTrans.x / 10.f;
		float Xangle = mouseTrans.y / 10.f;
		cameraXangle += Xangle;
		cameraYangle += Yangle;

		Matrix44 cameraMat = Matrix44::MakeRotationForEulerZXY(Vec3(cameraXangle, cameraYangle, 0.f), m_cameraPos);
		m_camera->SetModelMatrix(cameraMat);

		DebugRenderMessage(0.f, Rgba::RED, Rgba::RED, "camerapos:(%f ,%f, %f) Xangle:%f Yangle:%f ", m_cameraPos.x, m_cameraPos.y, m_cameraPos.z, cameraXangle, cameraYangle);
	}
	
	// Update light configs
	AdjustAmbient(deltaSeconds);

	// Update garbage entities
	DeleteGarbageEntities();

	

	// Change direction light
	xzAngle += deltaSeconds * 30.f;
	g_theRenderer->SetDirLightDir(Vec3(CosDegrees(xzAngle), -1.f, SinDegrees(xzAngle)));

	// Gameplay Update
	m_curMap->Update(deltaSeconds);

	// Update HUD
	int health1 = m_curMap->PlayerA()->GetCurrentHealth();
	int maxHealth1 = m_curMap->PlayerA()->m_attribe.maxHealth;
	float ratio1 = health1 / (float)maxHealth1;
	CPUMesh healthBar1 = CPUMesh();
	AABB2 barBox1 = AABB2(Vec2(3.f, 94.f), Vec2(3.f + 40.f * ratio1, 99.f));
	CPUMeshAddBox2D(&healthBar1, barBox1, Rgba::PINK_TAIKOH, Vec2::ZERO, Vec2::ONE);
	delete m_healthBar;
	m_healthBar = new GPUMesh(g_theRenderer->GetCTX());
	m_healthBar->CreateFromCPUMesh(&healthBar1, VERTEX_TYPE_LIGHT);

	int health2 = m_curMap->PlayerB()->GetCurrentHealth();
	int maxHealth2 = m_curMap->PlayerB()->m_attribe.maxHealth;
	float ratio2 = health2 / (float)maxHealth2;
	CPUMesh healthBar2 = CPUMesh();
	AABB2 barBox2 = AABB2(Vec2(147.f - ratio2 * 40.f, 94.f), Vec2(147.f, 99.f));
	CPUMeshAddBox2D(&healthBar2, barBox2, Rgba::PINK_TAIKOH, Vec2::ZERO, Vec2::ONE);
	delete m_healthBar_2;
	m_healthBar_2 = new GPUMesh(g_theRenderer->GetCTX());
	m_healthBar_2->CreateFromCPUMesh(&healthBar2, VERTEX_TYPE_LIGHT);
}

void Game::Render()
{
	g_theRenderer->ClearScreen(Rgba(0.f,0.f,0.f,1.f));

	g_theRenderer->BeginCamera(*m_camera);

	g_theRenderer->ClearScreen(Rgba(0.1f,0.1f,0.1f,1.f));
	g_theRenderer->ClearDepthStencilTargetOnCamera(m_camera);
	g_theRenderer->BindMaterial(m_mat);

	// Render map
	m_curMap->Render();

	g_theRenderer->BindTextureViewWithSampler(0, g_assetLoader->CreateOrGetTextureViewFromFile("white"), g_assetLoader->CreateOrGetSampler("linear"));

	// Render HUD
	g_theRenderer->BeginCamera(*m_HUDCamera);
	g_theRenderer->BindShader(g_assetLoader->CreateOrGetShaderFromXMLFile("defaultShader"));
	g_theRenderer->BindModelMatrix(Matrix44::identity);
	g_theRenderer->DrawMesh(m_healthBarBg);
	g_theRenderer->DrawMesh(m_healthBarBg_2);
	g_theRenderer->DrawMesh(m_healthBarSlot);
	g_theRenderer->DrawMesh(m_healthBarSlot_2);
	g_theRenderer->DrawMesh(m_healthBar);
	g_theRenderer->DrawMesh(m_healthBar_2);


	DebugRenderScreen();
}

void Game::DeleteGarbageEntities()
{
	m_curMap->DeleteGarbageEntities();
}

void Game::HandleKeyPressed(unsigned char KeyCode)
{
	switch( KeyCode )
	{
	case 'N':
		break;
	case 'W':
		cameraMoveFront = true;
		break;
	case 'A':
		cameraMoveLeft = true;
		break;
	case 'S':
		cameraMoveBack = true;
		break;
	case 'D':
		cameraMoveRight = true;
		break;
	case 'K':
		g_theRenderer->SetDirLight(true);
		break;
	case 'L':
		g_theRenderer->SetDirLight(false);
		break;
	case 'M':
		g_theRenderer->SetFirstPointLightPos(m_cameraPos);
		break;
	case KEY_COMMA:
		SetAmbientChangeAmount(1.f);
		break;
	case KEY_FULLSTOP:
		SetAmbientChangeAmount(-1.f);
		break;
	case '0':
		g_theRenderer->SetPointLightNum(0);
		break;
	case '1':
		g_theRenderer->SetPointLightNum(1);
		break;
	case '2':
		g_theRenderer->SetPointLightNum(2);
		break;
	case '3':
		g_theRenderer->SetPointLightNum(3);
		break;
	case '4':
		g_theRenderer->SetPointLightNum(4);
		break;
	case KEY_F1:
		break;
	default:
		break;
	}
}

void Game::HandleKeyReleased(unsigned char KeyCode)
{
	switch( KeyCode )
	{
	case 'W':
		cameraMoveFront = false;
		break;
	case 'A':
		cameraMoveLeft = false;
		break;
	case 'S':
		cameraMoveBack = false;
		break;
	case 'D':
		cameraMoveRight = false;
		break;
	case 'M':
		DebugRenderMessage(5.f, Rgba::RED, Rgba::YELLOW, "Set PointLightPos");
		break;
	case KEY_COMMA:
		SetAmbientChangeAmount(0.f);
		break;
	case KEY_FULLSTOP:
		SetAmbientChangeAmount(0.f);
		break;
	default:
		break;
	}
}

void Game::LoadResources()
{
	// Load Projectile Definition
	ProjectileDef* apple = new ProjectileDef();
	m_projectileInfo["Apple"] = apple;

	// Load Skill Definition
	SkillDefinition* appleAttack = new SkillDefinition(apple);
	m_skillInfo["Newton_0"] = appleAttack;

	// Load Player Attrib
                                                                                                                                  	// TBDs

	// Load voxel animation definition
	VoxelAnimDef* newAnim = new VoxelAnimDef();

	tinyxml2::XMLDocument animDefXML;
	animDefXML.LoadFile("Data/Animations/Voxel/_Template.anim");
	XmlElement* animRoot = animDefXML.RootElement();

	std::string animId = ParseXmlAttribute(*animRoot, "id", "");
	int frameNum = ParseXmlAttribute(*animRoot, "frameNum", 2);
	//	easing = "null" TBD
	float animTime = ParseXmlAttribute(*animRoot, "animTime", 3.f);

	newAnim->SetAnimTime(animTime);

	XmlElement* frameDef = animRoot->FirstChildElement();

	for(int i = 0; i < frameNum; i++)
	{
		float ratio = ParseXmlAttribute(*frameDef, "time", 1.0f);
		Vec3 pos = ParseXmlAttribute(*frameDef, "pos", Vec3(0.f,0.f,0.f));
		float scale = ParseXmlAttribute(*frameDef, "scale", 1.0f);
		Vec3 rotation = ParseXmlAttribute(*frameDef, "rotation", Vec3(0.f,0.f,0.f));
		Rgba tint = ParseXmlAttribute(*frameDef, "tint", Rgba::WHITE);

		VoxelAnimFrame newframe;
		newframe.frameRatio = ratio;
		newframe.pos = pos;
		newframe.rotation = rotation;
		newframe.scale = scale;
		newframe.tint = tint;

		newAnim->AddFrame(newframe);
		frameDef = frameDef->NextSiblingElement();
	}
	
	m_animInfo[animId] = newAnim;
}

void Game::AdjustAmbient(float deltaTime)
{
	ambientLightVolume += deltaTime * ambientChangeAmount;
	ambientLightVolume = Clamp(ambientLightVolume, 0.f, 1.f);
	//DebugRenderMessage(0.f, Rgba::WHITE, Rgba::WHITE, "AmbientVolume: %f", ambientLightVolume);
	g_theRenderer->SetAmbientLight(ambientLightVolume);
}

void Game::SetAmbientChangeAmount(float amount)
{
	ambientChangeAmount = amount;
}
