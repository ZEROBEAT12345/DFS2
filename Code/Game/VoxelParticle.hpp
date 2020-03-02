#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec3.hpp"

#define MAX_PARTICLE_FRAME 3

class Entity;

struct VoxelParticle
{


public:
	Entity* owner;
	Entity* target;
	Vec3 pos[MAX_PARTICLE_FRAME];
	Rgba color[MAX_PARTICLE_FRAME];
	float age = 0.f;
	float lifeSpan = 10.f;
	float impulseToOwnerRatio[MAX_PARTICLE_FRAME];
	float impulseToTargetRatio[MAX_PARTICLE_FRAME];
};