#include "Engine/Render/AssetLoader.hpp"
#include "Engine/Render/ImmediateRenderer.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Game/Projectile.hpp"
#include "Game/ProjectileDef.hpp"

extern AssetLoader* g_assetLoader;
extern ImmediateRenderer* g_theRenderer;

void Projectile::Initialize()
{
	m_mesh = g_assetLoader->CreateOrGetMeshFromFile(m_def->meshName.c_str());
}

void Projectile::Update(float deltaSeconds)
{
	if (m_isDead)
		return;

	m_lifespan += deltaSeconds;
	if(m_lifespan > m_def->existTime)
	{
		Die();
	}

	Vec2 DeltaPosition;
	DeltaPosition = deltaSeconds * Vec2::MakeFromPolarDegrees(m_forwardAngle, m_def->velocity);

	m_pos += DeltaPosition;
}

void Projectile::Render()
{
	Matrix44 curMat = Matrix44::MakeYRotationDegrees(-m_forwardAngle);
	curMat.SetTranslation(Vec3(m_pos.x, m_height, m_pos.y));
	g_theRenderer->BindModelMatrix(curMat);
	g_theRenderer->DrawMesh(m_mesh);
}

void Projectile::Die()
{
	m_isDead = true;
}