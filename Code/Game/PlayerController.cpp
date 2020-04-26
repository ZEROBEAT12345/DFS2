#include "Engine/Render/Utils/CPUMesh.hpp"
#include "Engine/Render/Utils/GPUMesh.hpp"
#include "Engine/Render/ImmediateRenderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Physics/PhysicsSystem.hpp"
#include "Game/PlayerController.hpp"
#include "Game/VoxelMesh.hpp"
#include "Game/SkillDefinition.hpp"
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/VoxelAnimator.hpp"
#include "Game/VoxelAnimDef.hpp"

extern ImmediateRenderer* g_theRenderer;
extern InputSystem* g_theInputSystem; 
extern PhysicsSystem* g_thePhysicSystem;
extern AudioSystem* g_theAudio;
using namespace DebugRender;

PlayerController::~PlayerController()
{
	delete m_body;
	delete m_bodyVoxel;

	delete m_hand;
	delete m_handVoxel;
}

void PlayerController::BeginFrame()
{
	// Handle input here
	// TBD
}

void PlayerController::Update(float deltaSeconds)
{
	// Update Skill cooldown
	for(int i = 0; i < SKILL_NUM; i++)
	{
		if(m_skillCoolDown[i] > 0.f)
		{
			m_skillCoolDown[i] -= deltaSeconds;
			if (m_skillCoolDown[i] < 0.f)
				m_skillCoolDown[i] = 0.f;
		}
	}

	if(m_isDash)
	{
		m_dashCount += deltaSeconds;
		if(m_dashCount > m_dashMaxTime)
		{
			m_isDash = false;
			m_isFrozen = false;
		}

		// update dash here
		Vec2 DeltaPosition;
		DeltaPosition = deltaSeconds * Vec2::MakeFromPolarDegrees(m_forwardAngle, m_attribe.movingSpeed);

		m_pos += DeltaPosition;
	}
	else
	{
		HandleJoystickInput(deltaSeconds);
	}
	m_bodyAnimator->Update(deltaSeconds);
}

void PlayerController::Render() const
{
	Matrix44 characterMat = Matrix44::MakeYRotationDegrees(-m_forwardAngle);
	characterMat.SetTranslation(Vec3(m_pos.x, 0.f, m_pos.y));

	g_theRenderer->BindModelMatrix(characterMat);
	g_theRenderer->DrawMesh(m_body);

	Matrix44 lefHandMat = Matrix44::MakeTranslation3D(Vec3(3.f, 8.f, 5.f));
	Matrix44 rightHandMat = Matrix44::MakeTranslation3D(Vec3(3.f, 8.f, -5.f));

	if (m_bodyAnimator->IsPlaying())
	{
		const VoxelAnimFrame& curFrame = m_bodyAnimator->GetCurAnimFrame();

		Matrix44 animMat = Matrix44::MakeRotationForEulerZXY(curFrame.rotation, curFrame.pos);

		lefHandMat = lefHandMat * animMat;
		//rightHandMat = rightHandMat * animMat;
	}

	g_theRenderer->BindModelMatrix(characterMat * lefHandMat);
	g_theRenderer->DrawMesh(m_hand);

	g_theRenderer->BindModelMatrix(characterMat * rightHandMat);
	g_theRenderer->DrawMesh(m_hand);
}

void PlayerController::Initialize()
{
	m_bodyVoxel = new VoxelMesh();
	m_body = new GPUMesh(g_theRenderer->GetCTX());

	m_handVoxel = new VoxelMesh();
	m_hand = new GPUMesh(g_theRenderer->GetCTX());

	m_bodyAnimator = new VoxelAnimator();
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
	if (m_isFrozen)
		return;

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

	KeyButtonState Xstate = controller.GetButtonState(XBOX_BUTTON_ID_X);
	if(Xstate.WasJustPressed())
	{
		UseSkill(SKILL_NORMAL_ATTACK);
	}

	KeyButtonState Ystate = controller.GetButtonState(XBOX_BUTTON_ID_Y);
	if (Ystate.WasJustPressed())
	{
		UseSkill(SKILL_1);
	}

	KeyButtonState Astate = controller.GetButtonState(XBOX_BUTTON_ID_A);
	if (Astate.WasJustPressed())
	{
		UseSkill(SKILL_2);
	}
}

// Gameplay
void PlayerController::UseSkill(int skillID)
{
	if (m_skillCoolDown[skillID] > 0.f)
		return;

	m_skills[skillID]->Cast(this, m_curMap); 
	m_skillCoolDown[skillID] = m_skills[skillID]->cooldown;
	m_bodyAnimator->PlayAnimation(m_attackAnim);
	m_game->m_soundPlaybackList[SOUND_TYPE_PLAYER_SHOOT] = g_theAudio->PlaySound(m_game->m_soundList[SOUND_TYPE_PLAYER_SHOOT]);
}

void PlayerController::GetDamage(int damage)
{
	if (m_isDead)                            
		return;

	m_curHealth -= damage;
	m_game->m_soundPlaybackList[SOUND_TYPE_PLAYER_HIT] = g_theAudio->PlaySound(m_game->m_soundList[SOUND_TYPE_PLAYER_HIT]);

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