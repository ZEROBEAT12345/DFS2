#include "Engine/Render/Utils/CPUMesh.hpp"
#include "Engine/Render/Utils/GPUMesh.hpp"
#include "Engine/Render/ImmediateRenderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "PlayerController.hpp"
#include "Game/VoxelMesh.hpp"

extern ImmediateRenderer* g_theRenderer;
extern InputSystem* g_theInputSystem;

PlayerController::~PlayerController()
{
	delete m_mesh;
	delete m_voxel;
}

void PlayerController::BeginFrame()
{

}

void PlayerController::Update(float deltaSeconds)
{
	HandleJoystickInput(deltaSeconds);
}

void PlayerController::Render()
{
	Matrix44 curMat = Matrix44::MakeYRotationDegrees(-m_forwardAngle);
	curMat.SetTranslation(Vec3(m_pos.x, 0.f, m_pos.y));
	g_theRenderer->BindModelMatrix(curMat);
	g_theRenderer->DrawMesh(m_mesh);
}

void PlayerController::Initialize()
{
	m_voxel = new VoxelMesh();
	m_mesh = new GPUMesh(g_theRenderer->GetCTX());
}

void PlayerController::AddModel(std::string modelPath)
{
	m_voxel->LoadFromFiles(modelPath);
	CPUMesh* modelMesh = m_voxel->GenerateMesh();
	m_mesh->CreateFromCPUMesh(modelMesh, VERTEX_TYPE_LIGHT);
}

// Input
void PlayerController::SetFreezeInput(bool isFrezze)
{

}

void PlayerController::HandleJoystickInput(float deltaSeconds)
{
	if (m_controllerID < 0)
		return;

	const XboxController& controller = g_theInputSystem->GetXboxController(m_controllerID);
	if (!controller.IsConnected())
		return;

	// Apply turning from left stick IF it is actively being used
	const AnalogJoystick& leftStick = controller.GetLeftJoystick();
	float leftStickMagnitude = leftStick.GetMagnitude();
	if (leftStickMagnitude > 0.f) // This is already deadzone-corrected; so anything non-zero means “significant movement?(outside the inner dead zone)
	{
		m_forwardAngle = leftStick.GetAngleDegrees();
		DebuggerPrintf("Forward angle: %f\n", m_forwardAngle);

		Vec2 DeltaPosition;
		DeltaPosition = deltaSeconds * Vec2::MakeFromPolarDegrees(m_forwardAngle, m_attribe.movingSpeed * leftStickMagnitude);

		m_pos += DeltaPosition;
	}
}

// Gameplay
void PlayerController::UseSkill(int skillID)
{

}