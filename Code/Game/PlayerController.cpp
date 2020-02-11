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
	delete m_body;
	delete m_bodyVoxel;
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
	g_theRenderer->DrawMesh(m_body);

	Matrix44 leftHandTranslate = Matrix44::MakeTranslation3D(Vec3(3.f, 8.f, 5.f));
	Matrix44 rightHandTranslate = Matrix44::MakeTranslation3D(Vec3(3.f, 8.f, -5.f));

	g_theRenderer->BindModelMatrix(curMat * leftHandTranslate);
	g_theRenderer->DrawMesh(m_hand);

	g_theRenderer->BindModelMatrix(curMat * rightHandTranslate);
	g_theRenderer->DrawMesh(m_hand);
}

void PlayerController::Initialize()
{
	m_bodyVoxel = new VoxelMesh();
	m_body = new GPUMesh(g_theRenderer->GetCTX());

	m_handVoxel = new VoxelMesh();
	m_hand = new GPUMesh(g_theRenderer->GetCTX());
}

void PlayerController::AddModel(std::string bodyModel, std::string handModel)
{
	m_bodyVoxel->LoadFromFiles(bodyModel);
	CPUMesh* modelMesh = m_bodyVoxel->GenerateMesh();
	m_body->CreateFromCPUMesh(modelMesh, VERTEX_TYPE_LIGHT);

	m_handVoxel->LoadFromFiles(handModel);
	CPUMesh* handMesh = m_handVoxel->GenerateMesh();
	m_hand->CreateFromCPUMesh(handMesh, VERTEX_TYPE_LIGHT);
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