#include "Game/VoxelParticle.hpp"
#include "Engine/Math/Algorithms/MathUtils.hpp"

void VoxelParticle:: Update(float deltaSeconds)
{
	if(isAlive)
	{
		age += deltaSeconds;
		if(age > lifeSpan)
		{
			isAlive = false;
			return;
		}

		Vec3 gravity = Vec3(0.f, 0.f, -9.8f);

		float curtimeRatio = age / lifeSpan;
		float curGravityRatio;
		if(curtimeRatio < framePoint)
		{
			float lerpRatio = curtimeRatio / framePoint;
			curGravityRatio = Interpolation(gravityRatio[0], gravityRatio[1], lerpRatio);
		}
		else
		{
			float lerpRatio = (curtimeRatio - framePoint) / (1.f - framePoint);
			curGravityRatio = Interpolation(gravityRatio[1], gravityRatio[2], lerpRatio);
		}
		

		Vec3 gravityT = gravity * curGravityRatio * deltaSeconds;
		velocity += gravityT;

		Vec3 translation = velocity * deltaSeconds;
		pos += translation;
	}
}

Rgba VoxelParticle::GetcurColor()
{
	float curtimeRatio = age / lifeSpan;
	Rgba curColor;
	if (curtimeRatio < framePoint)
	{
		float lerpRatio = curtimeRatio / framePoint;
		curColor = Lerp(color[0], color[1], lerpRatio);
	}
	else
	{
		float lerpRatio = (curtimeRatio - framePoint) / (1.f - framePoint);
		curColor = Lerp(color[1], color[2], lerpRatio);
	}

	return curColor;
}