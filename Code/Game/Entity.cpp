#include "Entity.hpp"
#include "Engine/Math/Algorithms/MathUtils.hpp"
#include "Engine/Render/Frame/RenderContext.hpp"
#include "Engine/Render/Utils/VertexUtils.hpp"

#define NUM_DISC_TRIANGLES 16
#define NUM_DISC_VERTS 48

Entity::~Entity()
{

}

extern RenderContext* g_theRenderer;

void Entity::DeveloperRender() const
{
	VertexPCU TempPhysicDiscVertex[NUM_DISC_TRIANGLES];
	VertexPCU TempPhysicDiscVertices[NUM_DISC_VERTS];
	VertexPCU TempCosmeticDiscVertex[NUM_DISC_TRIANGLES];
	VertexPCU TempCosmeticDiscVertices[NUM_DISC_VERTS];

	VertexPCU ZeroPoint = VertexPCU(Vec3(0.f, 0.f, 0.f), Rgba(0.4f, 0.4f, 0.4f, 1.0f), Vec2(0.f, 0.f));
	for(int i = 0; i < NUM_DISC_TRIANGLES; i++)
	{
		Vec2 Pos = Vec2(0.f, 0.f);
		Pos.x = CosDegrees(360.f / NUM_DISC_TRIANGLES * i) * m_cosmeticRadius;
		Pos.y = SinDegrees(360.f / NUM_DISC_TRIANGLES * i) * m_cosmeticRadius;

		TempCosmeticDiscVertex[i].m_position = Vec3(Pos.x, Pos.y, 0.f);
		TempCosmeticDiscVertex[i].m_color = Rgba(1.f, 0.f, 1.f, 1.f);
		TempCosmeticDiscVertex[i].m_uvTexCoords = Vec2(0.f, 0.f);

		Pos.x = CosDegrees(360.f / NUM_DISC_TRIANGLES * i) * m_physicsRadius;
		Pos.y = SinDegrees(360.f / NUM_DISC_TRIANGLES * i) * m_physicsRadius;

		TempPhysicDiscVertex[i].m_position = Vec3(Pos.x, Pos.y, 0.f);
		TempPhysicDiscVertex[i].m_color = Rgba(0.f, 1.f, 1.f, 1.f);
		TempPhysicDiscVertex[i].m_uvTexCoords = Vec2(0.f, 0.f);
	}


	for(int i = 0; i < NUM_DISC_TRIANGLES; i++)
	{
		TempCosmeticDiscVertices[i * 3] = ZeroPoint;
		TempCosmeticDiscVertices[i * 3 + 1] = TempCosmeticDiscVertex[i];

		TempPhysicDiscVertices[i * 3] = ZeroPoint;
		TempPhysicDiscVertices[i * 3 + 1] = TempPhysicDiscVertex[i];

		if(i == NUM_DISC_TRIANGLES - 1)
		{
			TempCosmeticDiscVertices[i * 3 + 2] = TempCosmeticDiscVertex[0];
			TempPhysicDiscVertices[i * 3 + 2] = TempPhysicDiscVertex[0];
		}	
		else
		{
			TempCosmeticDiscVertices[i * 3 + 2] = TempCosmeticDiscVertex[i + 1];
			TempPhysicDiscVertices[i * 3 + 2] = TempPhysicDiscVertex[i + 1];
		}
			
	}

	TransformVertexPCUArrayXY(NUM_DISC_VERTS, TempCosmeticDiscVertices, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray( NUM_DISC_VERTS, TempCosmeticDiscVertices);

	TransformVertexPCUArrayXY(NUM_DISC_VERTS, TempPhysicDiscVertices, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray( NUM_DISC_VERTS, TempPhysicDiscVertices);
}

Vec2 Entity::GetPosition() const
{
	return m_position;
}

Vec2 Entity::GetForwardVector() const
{
    return Vec2(1.f, 0.f).GetRotatedDegrees(m_orientationDegrees); 
}

float Entity::GetOrientationAngle() const
{
	return m_orientationDegrees;
}

bool Entity::IsOffscreen(float Width, float Height) const
{
	return m_position.x >= Width + m_cosmeticRadius || m_position.x <= 0.f - m_cosmeticRadius || m_position.y >= Height + m_cosmeticRadius || m_position.y <= 0.f - m_cosmeticRadius;
}

bool Entity::IsAlive() const
{
	return !m_isDead;
}

bool Entity::IsGarbage() const
{
	return m_isGarbage;
}

void Entity::SetVelocity(Vec2 velocity)
{
	m_velocity = velocity;
}

void Entity::ChangeVelocity(Vec2 velocity)
{
	m_velocity += velocity;
}