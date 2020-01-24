#pragma once
#include "Engine/Render/Frame/RenderContext.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

class Game;

class Entity
{
public:
	Entity() = default;
	~Entity();

	virtual void Update(float deltaSeconds) = 0;
	virtual void Render() const = 0;
	virtual void Die() = 0;
    
	void DeveloperRender() const;
	Vec2 GetPosition() const;
	Vec2 GetForwardVector() const;
	float GetOrientationAngle() const;
	bool IsOffscreen(float Width, float Height) const;
	bool IsAlive() const;
	bool IsGarbage() const;

	void SetVelocity(Vec2 velocity);
	void ChangeVelocity(Vec2 velocity);

protected:
	Game *m_Game;
	Vec2 m_position;
	Vec2 m_velocity;
	float m_physicsRadius;
	float m_cosmeticRadius;
	float m_orientationDegrees;
	float m_angularVelocity;
	bool m_isDead;
	bool m_isGarbage;
	//float m_uniformScale; 

};