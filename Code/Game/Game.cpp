#include "Game/Game.hpp"
#include "GameCommon.hpp"
#include "Game/VoxelMesh.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Debug/MemoryTrack.hpp"
#include "Engine/Core/Debug/Log.hpp"
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
	m_mat = g_assetLoader->CreateOrGetMaterialFromXMLFile("Data/Materials/couch.mat");
	m_mat = g_assetLoader->CreateOrGetMaterialFromXMLFile("Data/Materials/test.mat");
	m_mat = g_assetLoader->CreateOrGetMaterialFromXMLFile("Data/Materials/voxel.mat");

	// Bind event functions
	EventArgs newEventArgs = EventArgs();
	g_theEventSystem->SubscribeEventCallbackFunction("test", TestEvent);
	g_theEventSystem->SubscribeEventCallbackFunction("test", TestEvent2);
	g_theEventSystem->FireEvent("test");
	g_theEventSystem->FireEvent("test", newEventArgs);

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
	CPUMesh mesh;
	AABB3 cube = AABB3(Vec3(-3.f, -3.f, -3.f), Vec3(3.f, 3.f, 3.f));
	CPUMeshAddCube(&mesh, cube);
	m_cube = new GPUMesh(g_theRenderer->GetCTX());
	m_cube->CreateFromCPUMesh(&mesh, VERTEX_TYPE_LIGHT);
	float time = (float)GetCurrentTimeSeconds();
	Matrix44 cubeModelMat = Matrix44::MakeYRotationDegrees(ConvertRadiansToDegrees(time));
	//Matrix44 cubeModelMat = Matrix44::identity;
	cubeModelMat.SetTranslation(Vec3(-7.f, 0.f, 12.f));

	CPUMesh mesh2;
	CPUMeshAddUVSphere(&mesh2, Vec3(0.f, 0.f, 0.f), 3, 360, 180);
	m_sphere = new GPUMesh(g_theRenderer->GetCTX());
	m_sphere->CreateFromCPUMesh(&mesh2, VERTEX_TYPE_LIGHT); // we won't be updated this;
	//float time = (float)GetCurrentTimeSeconds();
	//Matrix44 sphereModelMat = Matrix44::MakeYRotationDegrees(ConvertRadiansToDegrees(time));
	Matrix44 sphereModelMat = Matrix44::identity;
	sphereModelMat.SetTranslation(Vec3(7.f, 0.f, 12.f));

	CPUMesh mesh3;
	CPUMeshAddBox2D(&mesh3, AABB2(Vec2(-3.f, -3.f), Vec2(3.f, 3.f)), Rgba::WHITE, Vec2::ZERO, Vec2::ONE);
	m_quad = new GPUMesh(g_theRenderer->GetCTX());
	m_quad->CreateFromCPUMesh(&mesh3, VERTEX_TYPE_LIGHT);
	Matrix44 quadModelMat = Matrix44::identity;
	quadModelMat.SetTranslation(Vec3(0.f, 0.f, 12.f));

	std::string vMeshPath[4] =
	{
		"Data/Models/Ply/chr_sword.ply",
		"Data/Models/Ply/chr_knight.ply",
		"Data/Models/Ply/chr_sword.ply",
		"Data/Models/Ply/chr_knight.ply"
	};

	for(int i = 0 ; i< 4; i++)
	{
		m_charVoxel[i] = new VoxelMesh();
		m_charVoxel[i]->LoadFromFiles(vMeshPath[i]);
		m_vMesh[i] = new GPUMesh(g_theRenderer->GetCTX());
		CPUMesh* vMesh = m_charVoxel[i]->GenerateMesh();
		m_vMesh[i]->CreateFromCPUMesh(vMesh, VERTEX_TYPE_LIGHT);
	}

	m_terrainVoxel = new VoxelMesh();
	m_terrainVoxel->LoadFromFiles("Data/Models/Ply/terrain.ply");
	m_tMesh = new GPUMesh(g_theRenderer->GetCTX());
	CPUMesh* tMesh = m_terrainVoxel->GenerateMesh();
	m_tMesh->CreateFromCPUMesh(tMesh, VERTEX_TYPE_LIGHT);

	// Initialize camera config
	cameraMovingDir = Vec3::ZERO;
	cameraMovingSpeed = 10.0f;

	TextureView *rtv = g_theRenderer->GetDefaultRTV();
	m_camera = new Camera();
	//m_camera->SetPerspectiveProjection(90.f, g_gameConfigBackround.GetValue("windowAspect", 1.777777f), 0.1f, 1000.f);
	m_camera->SetPerspectiveProjection(90.f, windowAspect, 0.1f, 1000.f);
	m_camera->SetColorTargetView(rtv);

	// Intialize light config
	g_theRenderer->SetAmbientLight(1.0);

	// Initialize projector
	m_projector = new Camera();
	//m_projector->SetPerspectiveProjection(90, g_gameConfigBackround.GetValue("windowAspect", 1.777777f), 0.1f, 1000.f);
	m_projector->SetPerspectiveProjection(90, windowAspect, 0.1f, 1000.f);
	m_projector->SetColorTargetView(rtv);
	Matrix44 cameraMat = Matrix44::MakeRotationForEulerXZY(Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f));
	m_projector->SetModelMatrix(cameraMat);

	SetProjector(m_projector);
	g_theRenderer->BindTextureViewWithSampler(1, g_assetLoader->CreateOrGetTextureViewFromFile("Data/Images/Test_StbiFlippedAndOpenGL.png"), 
		g_assetLoader->CreateOrGetSampler("point"));

}

void Game::Shutdown()
{
	delete m_cube;
	m_cube = nullptr;

	delete m_sphere;
	m_sphere = nullptr;

	delete m_quad;
	m_quad = nullptr;

	for (int i = 0; i < 4; i++)
	{
		delete m_vMesh[i];
		m_vMesh[i] = nullptr;
	}

	delete m_tMesh;
	m_tMesh = nullptr;

	delete m_projector;
	m_projector = nullptr;

	delete m_camera;
	m_camera = nullptr;
}

void Game::Update(float deltaSeconds)
{

	// Update camera configs
	//DebugRenderScreenTextf(0.f, Vec2(1150.f, 210.f), Vec2(.5f, .5f), 10.f, 0.8f, Rgba::WHITE, Rgba::WHITE, "CurTime: %f", GetCurrentTimeSeconds());
	//DebugRenderMessage(0.f, Rgba::WHITE, Rgba::WHITE, "CurTime: %f", GetCurrentTimeSeconds());

	cameraMovingDir = Vec3::ZERO;
	if (cameraMoveFront)
		cameraMovingDir += Vec3(0.f, 0.f, 1.f);

	if (cameraMoveBack)
		cameraMovingDir += Vec3(0.f, 0.f, -1.f);

	if (cameraMoveLeft)
		cameraMovingDir += Vec3(-1.f, 0.f, 0.f);

	if (cameraMoveRight)
		cameraMovingDir += Vec3(1.f, 0.f, 0.f);

	Matrix44 cameraRotateMat = Matrix44::MakeRotationForEulerXZY(Vec3(cameraXangle, cameraYangle, 0.f), Vec3::ZERO);
	Vec3 cameraLocalTrans = cameraMovingDir * cameraMovingSpeed * deltaSeconds;
	Vec4 cameraWorldTrans = cameraRotateMat * Vec4(cameraLocalTrans.x, cameraLocalTrans.y, cameraLocalTrans.z, 0.f);
	m_cameraPos += Vec3(cameraWorldTrans.x, cameraWorldTrans.y, cameraWorldTrans.z);
	
	// Update light configs
	AdjustAmbient(deltaSeconds);

	// Update garbage entities
	DeleteGarbageEntities();

	//g_theEventSystem->FireEvent("allocCount");
	DebugRenderMessage(0.f, Rgba::RED, Rgba::RED, GetSizeString(MemTrackGetLiveByteCount()).c_str());
}

void Game::Render()
{
	g_theRenderer->ClearScreen(Rgba(0.f,0.f,0.f,1.f));

	Matrix44 tonemapMat = Matrix44::identity;
	tonemapMat.s[ix] = 0.33f;
	tonemapMat.s[iy] = 0.33f;
	tonemapMat.s[iz] = 0.33f;
	tonemapMat.s[jx] = 0.33f;
	tonemapMat.s[jy] = 0.33f;
	tonemapMat.s[jz] = 0.33f;
	tonemapMat.s[kx] = 0.33f;
	tonemapMat.s[ky] = 0.33f;
	tonemapMat.s[kz] = 0.33f;
	float tonemapS = 1.f;
	TonemapBufferT curTonemapStruct = TonemapBufferT{
		tonemapMat,
		tonemapS,
		Vec3::ZERO
	};
	//m_tonemapBuffer->CopyCPUToGPU(&curTonemapStruct, sizeof(TonemapBufferT));

	// Update camera info
	IntVec2 mouseTrans = g_theWindow->GetClientMouseRelativePositon();
	float Yangle =  mouseTrans.x / 10.f;
	float Xangle =  mouseTrans.y / 10.f;
	cameraXangle += Xangle;
	cameraYangle += Yangle;

	Matrix44 cameraMat = Matrix44::MakeRotationForEulerXZY(Vec3(cameraXangle, cameraYangle, 0.f), m_cameraPos);
	m_camera->SetModelMatrix(cameraMat);

	//m_testScene->SetCamera(m_camera);

	//g_theRenderer->DrawScene(m_testScene);

	g_theRenderer->BeginCamera(*m_camera);

	g_theRenderer->ClearScreen(Rgba(0.1f,0.1f,0.1f,1.f));
	g_theRenderer->ClearDepthStencilTargetOnCamera(m_camera);
	g_theRenderer->BindMaterial(m_mat);

	float time = (float)GetCurrentTimeSeconds();
	Matrix44 cubeModelMat = Matrix44::MakeYRotationDegrees(ConvertRadiansToDegrees(time));
	cubeModelMat.SetTranslation(Vec3(-7.f, 0.f, 12.f));

	Matrix44 sphereModelMat = Matrix44::identity;
	sphereModelMat.SetTranslation(Vec3(7.f, 0.f, 12.f));

	Matrix44 quadModelMat = Matrix44::identity;
	quadModelMat.SetTranslation(Vec3(0.f, 0.f, 12.f));

	Matrix44 vmat[4];
	for (int i = 0; i < 4; i++)
	{
		vmat[i].SetTranslation(Vec3(-24.f + 12.f * i, 0.f, 12.f));
		g_theRenderer->BindModelMatrix(vmat[i]);
		g_theRenderer->DrawMesh(m_vMesh[i]);
	}

	Matrix44 terrainModelMat = Matrix44::identity;
	terrainModelMat.SetTranslation(Vec3(0.f, -3.f, 12.f));
	g_theRenderer->BindModelMatrix(terrainModelMat);
	g_theRenderer->DrawMesh(m_tMesh);

	g_theRenderer->BindTextureViewWithSampler(0, g_assetLoader->CreateOrGetTextureViewFromFile("white"), g_assetLoader->CreateOrGetSampler("linear"));

	DebugRenderScreen();
}

void Game::DeleteGarbageEntities()
{
	
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

void Game::SetProjector(Camera* projector)
{
	m_projector = projector;

	ProjectorBufferT curProjectorStruct = ProjectorBufferT
	{
		m_projector->GetViewMatrix(),
		m_projector->GetProjectionMatrix(),
		m_projector->GetModelMatrix().GetVecT()
	};

	// Update buffer on mat
	//m_projectorBuffer->CopyCPUToGPU(&curProjectorStruct, sizeof(ProjectorBufferT));
}
