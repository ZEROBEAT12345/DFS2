#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec3.hpp"

#define MAX_PARTICLE_FRAME 3

class Entity;

struct VoxelParticle
{
	void Update(float deltaSeconds); // Update 
	Rgba GetcurColor();

public:
	bool isAlive = false;
	Entity* owner = nullptr;
	Entity* target = nullptr;
	Vec3 pos;
	Vec3 velocity;
	Rgba color[MAX_PARTICLE_FRAME];
	float framePointRatio = 0.5f;
	float ageSeconds = 0.f;                                                                         
	float lifeSpanSeconds = 10.f;
	float impulseToOwnerRatio[MAX_PARTICLE_FRAME];
	float impulseToTargetRatio[MAX_PARTICLE_FRAME];
	float gravityRatio[MAX_PARTICLE_FRAME];
	// add size
};