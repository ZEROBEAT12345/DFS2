#include "App.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Shapes/AABB2.hpp"
#include "Engine/Math/Algorithms/MathUtils.hpp"
#include "Engine/Math/Algorithms/RandomNumberGenerator.hpp"
#include "Engine/Render/Utils/VertexUtils.hpp"
#include "Engine/Render/Frame/RHIDeviceContext.hpp"
#include "Engine/Render/ImmediateRenderer.hpp"
#include "Engine/Render/AssetLoader.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Debug/UnitTest.hpp"
#include "Engine/Core/Debug/CallStack.hpp"
#include "Engine/Core/Debug/MemoryTrack.hpp"
#include "Engine/Core/Debug/Log.hpp"
#include "Engine/Core/Async/RingBuffer.hpp"
#include "Engine/Physics/PhysicsSystem.hpp"

#define DX11

#ifdef DX11
	#include "Engine/Render/D3DRenderer.hpp"
	#include "Engine/Render/Frame/D3D/D3DDeviceContext.hpp"
#else
	#ifdef VULKAN
		#include "Engine/Render/VulkanRenderer.hpp"
	#endif
#endif


App* g_theApp = nullptr;				// Created and owned by Main_Windows.cpp
extern WindowContext* g_theWindow;

BitmapFont* g_testFont = nullptr;
RandomNumberGenerator* g_random = nullptr;
RHIDeviceContext* g_context = nullptr;
AssetLoader* g_assetLoader = nullptr;
ImmediateRenderer* g_theRenderer = nullptr;		// Created and owned by the App
InputSystem* g_theInputSystem = nullptr;
AudioSystem* g_theAudio = nullptr;
PhysicsSystem* g_thePhysicSystem = nullptr;
Clock* g_gameClock = nullptr;
UnitTest* gAllTests = nullptr;
extern DevConsole* g_theDevConsole;
extern EventSystem* g_theEventSystem;

extern MPSCRingBuffer* g_ringBuffer;

using namespace DebugRender;
using namespace SystemClock;



UNITTEST("testUnitTest", "test", 0)
{
	DebuggerPrintf("UnitTest Success!!!\n");
	return true;
}

UNITTEST("testUnitTest2", "test", 1)
{
	DebuggerPrintf("UnitTest2 Success!!!\n");
	return true;
}

UNITTEST("testCallStack", "test", 0)
{
	std::vector<std::string> callStacks = CallstackToString(CallstackGet(1));
	if (callStacks.size() == 0)
		return false;
	else
	{
		for (std::string s : callStacks)
			DebuggerPrintf(s.c_str());
	}

	return true;
}

void TestLogHook()
{
	g_theDevConsole->PrintString("logPrinted", Rgba::RED);
}

App::App()
{

}

App::~App()
{

}

void App::Startup()
{
	LogSystemStartup("Data/testLog.txt");

	SystemClockStartup();
	g_gameClock = new Clock();
	g_gameClock->Dilate(1.f);

	g_random = new RandomNumberGenerator();

	// Render pipeline
#ifdef DX11
	g_context = new D3DDeviceContext();
	g_theRenderer = new D3DRenderer(g_context);
	g_assetLoader = new AssetLoader(g_context);
#else
	#ifdef VULKAN
	// TBD
	#endif
#endif

	g_context->Init(g_theWindow->m_hwnd);
	g_theRenderer->Startup();

	g_testFont = g_assetLoader->CreateOrGetBitmapFontFromFile("SquirrelFixedFont");

	g_theInputSystem = new InputSystem();
	g_theInputSystem->Startup();

	g_theAudio = new AudioSystem();

	g_theDevConsole = new DevConsole( g_testFont );
	g_theDevConsole->Startup();

	g_thePhysicSystem = new PhysicsSystem();

	g_thePhysicSystem->StartUp(g_gameClock);
	g_thePhysicSystem->SetGravity(Vec2(0.f, 0.f));

	g_theEventSystem = new EventSystem();

	g_theDebugRenderSystem = new DebugRenderSystem();
	DebugRenderSystemStartup(g_theRenderer);

	m_theGame = new Game();
	m_theGame->Startup();

	m_timeLastframebegan = 0.f;
	m_timeThisframebegan = 0.f;

	/*Test LogSystem*/
	//LogHook(TestLogHook);
	//AddLogFilter("Warning");

	//Logf("test", "Log System Initialize");
	//Logf("Test", "TestMessage 1");
	//LogFlush();
	//Logf("Warning", "TestMessage 2");
}

void App::Shutdown()
{
	m_theGame->Shutdown();
	delete m_theGame;
	m_theGame = nullptr;

	g_theDebugRenderSystem->Shutdown();
	delete g_theDebugRenderSystem;
	g_theDebugRenderSystem = nullptr;

	g_theRenderer->Shutdown();
	delete g_theRenderer;
	g_theRenderer = nullptr;

	delete g_assetLoader;
	g_assetLoader = nullptr;

	g_context->Cleanup();
	delete g_context;
	g_context = nullptr;

	delete g_theInputSystem;
	g_theInputSystem = nullptr;

	delete g_thePhysicSystem;
	g_thePhysicSystem = nullptr;

	LogSystemShutdown();
}

void App::RunFrame()
{
	BeginFrame();

	Update((float)GetSystemClockFrameTime());

	RenderFrame();

	EndFrame();
}

bool App::HandleKeyPressed(unsigned char keyCode )
{
	if(m_isDevConsoleOn)
	{
		g_theDevConsole->HandleKeyPressed(keyCode);
	}

	switch( keyCode )
	{
	case KEY_ESC:
		if (m_isDevConsoleOn)
			m_isDevConsoleOn = false;
		else
			m_isQuitting = true;
		break;
	case KEY_F4:
		m_isDevConsoleOn = !m_isDevConsoleOn;
		break;
	case KEY_F8:
		Restart();
		break;
	case KEY_F9:
		g_assetLoader->ReloadShaderStageWithPath("Data/Shaders/default_bitangentAsColor.1.hlsl");
		break;
	case 'P':
		m_isPaused = !m_isPaused;
		break;
	case 'T':
		m_isSlowMo = true;
		break;
	case 'N':
	case 'A':
	case 'W':
	case 'S':
	case 'D':
	case 'K':
	case 'L':
	case 'M':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case KEY_F1:
	case KEY_COMMA:
	case KEY_FULLSTOP:
	case KEY_LEFTARROW:
	case KEY_RIGHTARROW:
	case KEY_UPARROW:
	case KEY_SPACEBAR:
		if(!m_isDevConsoleOn)
			m_theGame->HandleKeyPressed(keyCode);
		break;
	}

	return false;
}

bool App::HandleKeyReleased( unsigned char keyCode )
{
	if (m_isDevConsoleOn)
	{
		g_theDevConsole->HandleKeyReleased(keyCode);
	}

	switch( keyCode )
	{
	case 'T':
		m_isSlowMo = false;
		break;
	case 'A':
	case 'W':
	case 'S':
	case 'D':
	case 'M':
	case KEY_COMMA:
	case KEY_FULLSTOP:
		if (!m_isDevConsoleOn)
			m_theGame->HandleKeyReleased(keyCode);
		break;
	}

	return false;
}

bool App::HandleQuitRequested()
{
	m_isQuitting = true;
	return false;
}

bool App::HandleCharRequested(unsigned char keyCode)
{
	if (!m_isDevConsoleOn)
		return false;

	if( (keyCode >= 'a' && keyCode <= 'z') || (keyCode >= 'A' || keyCode <= 'Z') || ( keyCode >= '0' || keyCode <= '9') || keyCode == KEY_SPACEBAR )
		g_theDevConsole->AddLetterToInputField(keyCode);
	return false;
}

void App::BeginFrame()
{
	SystemClockBeginFrame();

	g_thePhysicSystem->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theInputSystem->BeginFrame();
}

void App::Update( float deltaSeconds )
{
	if(m_isPaused)
		deltaSeconds = 0.f;

	if(m_isSlowMo)
		deltaSeconds *= 0.1f;

	g_thePhysicSystem->FixedUpdate();
	g_thePhysicSystem->PreRender();

	m_theGame->Update(deltaSeconds);

	g_theDebugRenderSystem->Update(deltaSeconds);

}

void App::RenderFrame() const
{

	m_theGame->Render();
	//g_theRenderer->EndCamera();


	// render the dev console contents

	if(m_isDevConsoleOn)
	{
		TextureView *rtv = g_theRenderer->GetDefaultRTV();

		Camera *HUDCamera = new Camera();
		HUDCamera->SetOrthographicProjection(Vec2(0.f, 0.f), Vec2(240.f, 180.f));
		HUDCamera->SetColorTargetView(rtv);
		Matrix44 cameraMat = Matrix44();
		cameraMat.SetTranslation(Vec3(0.f, 0.f, 0.f));
		HUDCamera->SetModelMatrix(cameraMat);
		g_theRenderer->BeginCamera(*HUDCamera);

		//g_theRenderer->BindTextureViewWithSampler(0, "Data/Images/Test_StbiFlippedAndOpenGL.png", SAMPLE_MODE_POINT);
		g_theDevConsole->Render(g_theRenderer, HUDCamera, 5.0f);

		g_theRenderer->EndCamera(*HUDCamera);
	}
}

void App::EndFrame()
{
	g_theDebugRenderSystem->EndFrame();
	g_theRenderer->EndFrame();
	g_theInputSystem->EndFrame();
}

void App::Restart()
{
	delete m_theGame;
	m_theGame = new Game();
	m_theGame->Startup();
}
