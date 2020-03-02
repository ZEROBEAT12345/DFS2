#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

class Game;

class Entity
{
public:
	Entity() = default;
	virtual ~Entity();

	virtual void Update(float deltaSeconds) = 0;
	virtual void Render() const = 0;
	virtual void Die() = 0;

protected:
	Game *m_Game;
};