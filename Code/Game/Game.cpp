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

	g_theDevConsole->PrintString("hello", Rgba(0.6f, 0.5f, 0.7f, 1.0f));

	g_theEventSystem->SubscribeEventCallbackFunction("allocCount", TrackMemoryCount);
	g_theEventSystem->SubscribeEventCallbackFunction("byteCount", TrackMemoryByteCount);
	g_theEventSystem->SubscribeEventCallbackFunction("logMemory", LogTrackMemory);
	g_theEventSystem->SubscribeEventCallbackFunction("AddFilter", AddFilter);
	g_theEventSystem->SubscribeEventCallbackFunction("TestLog", TestLog);

	// Initialize test scene
	m_terrainVoxel = new VoxelMesh();
	m_terrainVoxel->LoadFromFiles("Data/Models/Ply/terrain.ply");
	m_tMesh = new GPUMesh(g_theRenderer->GetCTX());
	CPUMesh* tMesh = m_terrainVoxel->GenerateMesh();
	m_tMesh->CreateFromCPUMesh(tMesh, VERTEX_TYPE_LIGHT);

	VoxelMesh* appleVoxel = new VoxelMesh();
	appleVoxel->LoadFromFiles("Data/Models/Ply/apple.ply");
	GPUMesh* aMesh = new GPUMesh(g_theRenderer->GetCTX());
	CPUMesh* appleMesh = appleVoxel->GenerateMesh();
	aMesh->CreateFromCPUMesh(appleMesh, VERTEX_TYPE_LIGHT);
	g_assetLoader->RegisterMesh("Apple", aMesh);

	// Initialize camera config
	cameraMovingDir = Vec3::ZERO;
	cameraMovingSpeed = 10.0f;

	TextureView *rtv = g_theRenderer->GetDefaultRTV();
	m_camera = new Camera();
	//m_camera->SetPerspectiveProjection(90.f, g_gameConfigBackround.GetValue("windowAspect", 1.777777f), 0.1f, 1000.f);
	m_camera->SetPerspectiveProjection(90.f, windowAspect, 0.1f, 1000.f);
	m_camera->SetColorTargetView(rtv);
	m_cameraPos = Vec3(0.f, 50.f, -50.f);

	// Initialize light config
	g_theRenderer->SetAmbientLight(1.0);

	LoadResources();

	// Initialize Map
	m_curMap = new Map();
	m_curMap->SetTerrain(m_tMesh);

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
		m_curMap->SetPlayer(i, newPlayer);
	}
}

void Game::Shutdown()
{

	delete m_curMap;
	m_curMap = nullptr;

	delete m_tMesh;
	m_tMesh = nullptr;

	delete m_camera;
	m_camera = nullptr;
}

void Game::Update(float deltaSeconds)
{
	// Update camera config
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
	
	// Update light configs
	AdjustAmbient(deltaSeconds);

	// Update garbage entities
	DeleteGarbageEntities();

	//g_theEventSystem->FireEvent("allocCount");
	DebugRenderMessage(0.f, Rgba::RED, Rgba::RED, GetSizeString(MemTrackGetLiveByteCount()).c_str());

	// Change direction light
	xzAngle += deltaSeconds * 30.f;
	g_theRenderer->SetDirLightDir(Vec3(CosDegrees(xzAngle), -1.f, SinDegrees(xzAngle)));

	// Gameplay Update
	m_curMap->Update(deltaSeconds);
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
