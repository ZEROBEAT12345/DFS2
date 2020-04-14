#include "Engine/Render/AssetLoader.hpp"
#include "Engine/Render/ImmediateRenderer.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Physics/PhysicsSystem.hpp"
#include "Game/Projectile.hpp"
#include "Game/ProjectileDef.hpp"
#include "Game/Map.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

extern AssetLoader* g_assetLoader;
extern ImmediateRenderer* g_theRenderer;
extern PhysicsSystem* g_thePhysicSystem;

void Projectile::Initialize()
{
	m_mesh = g_assetLoader->CreateOrGetMeshFromFile(m_def->meshName.c_str());

}

void Projectile::Update(float deltaSeconds)
{
	if (m_isDead)
		return;

	// Normal move
	Vec2 DeltaPosition = Vec2::ZERO;

	switch (m_pid)
	{
	case PROJECTILE_NEWTON_NORMAL_ATTACK:
		// life update
		m_lifespan += deltaSeconds;
		if (m_lifespan > m_def->existTime)
		{
			Die();
		}

		DeltaPosition = deltaSeconds * Vec2::MakeFromPolarDegrees(m_forwardAngle, m_def->velocity);
		m_pos += DeltaPosition;
		break;
	case PROJECTILE_NEWTON_SKILL_1:
		if(m_isAppleRainOn) // Update apple rain timer
		{
			m_appleRainCount += deltaSeconds;
			if (m_appleRainCount > m_appleRainMaxTime)
			{
				// Stop Apple rain
				m_isAppleRainOn = false;
				Die();
			}
			else
			{
				m_appleRainCount += deltaSeconds;
				if (m_appleRainCount >= 1.0f)
				{
					m_appleRainCount -= 1.0f;

					// Apply damage
					m_themap->DealAOEDamage(m_pos, m_appleRainRadius, m_appleRainDamage, true);
				}
			}

		}
		else // update 
		{
			// life update
			m_lifespan += deltaSeconds;
			if (m_lifespan > m_def->existTime)
			{
				Die();
			}

			DeltaPosition = deltaSeconds * Vec2::MakeFromPolarDegrees(m_forwardAngle, m_def->velocity);
			m_pos += DeltaPosition;
		}

		break;
	case PROJECTILE_NEWTON_SKILL_2:
		break;
	case PROJECTILE_JONES_NORMAL_ATTACK:
		break;
	case PROJECTILE_JONES_SKILL_1:
		break;
	case PROJECTILE_JONES_SKILL_2:
		break;
	case PROJECTILE_TYPE_NUM:
		break;
	default:
		break;
	}


}

void Projectile::Render()
{
	Matrix44 curMat;

	switch (m_pid)
	{
	case PROJECTILE_NEWTON_NORMAL_ATTACK:
		curMat = Matrix44::MakeYRotationDegrees(-m_forwardAngle);
		curMat.SetTranslation(Vec3(m_pos.x, m_height, m_pos.y));
		curMat = curMat * Matrix44::MakeUniformScale3D(m_def->size);
		g_theRenderer->BindModelMatrix(curMat);
		g_theRenderer->DrawMesh(m_mesh);
		break;
	case PROJECTILE_NEWTON_SKILL_1:
		
		if(m_isAppleRainOn)// Render apple rain ring
		{
			if(!m_appleRainRingMesh)
			{
				m_appleRainRingMesh = new GPUMesh(g_theRenderer->GetCTX());
				CPUMesh* arMesh = new CPUMesh();
				CPUMeshAddDisc2D(arMesh, Vec2::ZERO, m_appleRainRadius, Rgba(1.f, 0.f, 0.f, .9f), 20);
				m_appleRainRingMesh->CreateFromCPUMesh(arMesh, VERTEX_TYPE_LIGHT);
			}

			curMat = Matrix44::MakeXRotationDegrees(90.f);
			curMat.SetTranslation(Vec3(m_pos.x, 0.1f, m_pos.y));
			g_theRenderer->BindModelMatrix(curMat);
			g_theRenderer->DrawMesh(m_appleRainRingMesh);
		}
		else// Render normal bullet when apple rain is off
		{
			curMat = Matrix44::MakeYRotationDegrees(-m_forwardAngle);
			curMat.SetTranslation(Vec3(m_pos.x, m_height, m_pos.y));
			curMat = curMat * Matrix44::MakeUniformScale3D(m_def->size);
			g_theRenderer->BindModelMatrix(curMat);
			g_theRenderer->DrawMesh(m_mesh);
		}
		
		break;
	case PROJECTILE_NEWTON_SKILL_2:
		break;
	case PROJECTILE_JONES_NORMAL_ATTACK:
		break;
	case PROJECTILE_JONES_SKILL_1:
		break;
	case PROJECTILE_JONES_SKILL_2:
		break;
	case PROJECTILE_TYPE_NUM:
		break;
	default:
		break;
	}


}

void Projectile::Die()
{
	m_isDead = true;
}

void Projectile::OnHit()
{ 
	switch (m_pid)
	{
	case PROJECTILE_NEWTON_NORMAL_ATTACK:
		Die();
		break;
	case PROJECTILE_NEWTON_SKILL_1:
		if(m_isAppleRainOn)
		{
			//ERROR_AND_DIE("SHOULDN'T BE TRIGGERED!!!")

			// Do nothing
		}
		else
		{
			m_isAppleRainOn = true;
		}
		break;
	case PROJECTILE_NEWTON_SKILL_2:
		break;
	case PROJECTILE_JONES_NORMAL_ATTACK:
		break;
	case PROJECTILE_JONES_SKILL_1:
		break;
	case PROJECTILE_JONES_SKILL_2:
		break;
	case PROJECTILE_TYPE_NUM:
		break;
	default:
		break;
	}

}