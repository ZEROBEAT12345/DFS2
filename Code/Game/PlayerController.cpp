#include "Engine/Render/Utils/CPUMesh.hpp"
#include "Engine/Render/Utils/GPUMesh.hpp"
#include "Engine/Render/ImmediateRenderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/PlayerController.hpp"
#include "Game/VoxelMesh.hpp"
#include "Game/SkillDefinition.hpp"
#include "Game/Map.hpp"

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
	curMat.SetTranslation(Vec3(m_pos.x, 5.f, m_pos.y));
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
	m_isFrozen = isFrezze;
}

void PlayerController::HandleJoystickInput(float deltaSeconds)
{
	if (m_controllerID < 0)
		return;

	const XboxController& controller = g_theInputSystem->GetXboxController(m_controllerID);
	if (!controller.IsConnected())
		return;

	// Handle movement
	const AnalogJoystick& leftStick = controller.GetLeftJoystick();
	float leftStickMagnitude = leftStick.GetMagnitude();
	if (leftStickMagnitude > 0.f)
	{
		m_forwardAngle = leftStick.GetAngleDegrees();

		Vec2 DeltaPosition;
		DeltaPosition = deltaSeconds * Vec2::MakeFromPolarDegrees(m_forwardAngle, m_attribe.movingSpeed * leftStickMagnitude);

		m_pos += DeltaPosition;
	}

	// Handle Skill
	float rTrigger = controller.GetRightTrigger();

	KeyButtonState Xstate = controller.GetButtonState(XBOX_BUTTON_ID_X);
	if(Xstate.WasJustPressed())
	{
		UseSkill(SKILL_NORMAL_ATTACK);
	}
}

// Gameplay
void PlayerController::UseSkill(int skillID)
{
	m_skills[skillID]->Cast(this, m_curMap); 
}

void PlayerController::GetDamage(int damage)
{
	m_curHealth -= damage;
	
	if(m_curHealth <= 0)
	{
		m_curHealth = 0;
		Die();
	}

	
}

void PlayerController::Die()
{
	m_isDead = true;
}