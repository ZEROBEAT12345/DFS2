#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "App.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
//
//

extern App* g_theApp;				// Created and owned by Main_Windows.cpp
WindowContext* g_theWindow;

//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called by Windows whenever we ask it for notifications
//
bool WindowsMessageHandlingProcedure( void* windowHandle, uint wmMessageCode, uintptr_t wparam, uintptr_t lparam)
{
	UNUSED(lparam);
	UNUSED(windowHandle);
	switch( wmMessageCode )
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
		case WM_CLOSE:		
		{
			g_theApp->HandleQuitRequested();
			return true; // "Consumes" this message (tells Windows "okay, we handled it")
		}

		// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
		case WM_KEYDOWN:
		{
			unsigned char asKey = (unsigned char)wparam;

			// Tell the Input system about this key-pressed event
			g_theApp->HandleKeyPressed(asKey);
			return true;
		}

		// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
		case WM_KEYUP:
		{
			unsigned char asKey = (unsigned char) wparam;
            // Tell the Input system about this key-released event
			g_theApp->HandleKeyReleased(asKey);
			return true;
		}

		case WM_CHAR:
		{
			unsigned char asKey = (unsigned char) wparam;
			g_theApp->HandleCharRequested(asKey);
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
void Startup( HINSTANCE applicationInstanceHandle )
{
	UNUSED(applicationInstanceHandle);

	//tinyxml2::XMLDocument gameConfigDefsXML;
	//gameConfigDefsXML.LoadFile("Data/GameConfigs.xml");
	//g_gameConfigBackround.PopulateFromXmlElementAttributes(*gameConfigDefsXML.FirstChildElement());
	//int customHeight = g_gameConfigBackround.GetValue("windowHeight", 900);
	int customHeight = windowHeight;
	//float customAspect = g_gameConfigBackround.GetValue("windowAspect", 1.777777f);
	float customAspect = windowAspect;
	int customWidth = (int)(customHeight * customAspect);
	//std::string appName = g_gameConfigBackround.GetValue("windowsTitle", "default");
	std::string appName = "Protogame3D";
	
	g_theWindow = new WindowContext();
	g_theWindow->CreateClientWindow(appName.c_str(), customWidth, customHeight, &WindowsMessageHandlingProcedure);
	g_theWindow->SetMouseMode(MOUSE_MODE_RELATIVE);

	g_theApp = new App();
	g_theApp->Startup();

	
}

//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	// Destroy the global App instance
	g_theApp->Shutdown();
    delete g_theApp;			
    g_theApp = nullptr;
}

//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	UNUSED( commandLineString );
	Startup( applicationInstanceHandle );

	// Program main loop; keep running frames until it's time to quit
	while( !g_theApp->IsQuitting())
	{
		// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
		//

		g_theWindow->BeginFrame();

		g_theApp->RunFrame();
	}

	Shutdown();
	return 0;
}


