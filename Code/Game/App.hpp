#pragma once

#include "Game.hpp"

class App
{
public:
	App();
	~App();
	void Startup();
	void Shutdown();
	void RunFrame();

	bool IsQuitting() const { return m_isQuitting; }
	bool HandleKeyPressed( unsigned char keyCode );
	bool HandleKeyReleased( unsigned char keyCode );
	bool HandleQuitRequested();
	bool HandleCharRequested( unsigned char keyCode );

private:
	void BeginFrame();
	void Update( float deltaSeconds );
	void RenderFrame() const;
	void EndFrame();
	void Restart();

private:
	Game* m_theGame;
	bool m_isQuitting = false;
	bool m_isPaused = false;
	bool m_isSlowMo = false;
	bool m_isDevConsoleOn = false;

	double m_timeThisframebegan;
	double m_timeLastframebegan;
};
