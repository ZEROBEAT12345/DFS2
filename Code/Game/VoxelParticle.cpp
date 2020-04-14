#include "Game/VoxelParticle.hpp"
#include "Engine/Math/Algorithms/MathUtils.hpp"

void VoxelParticle:: Update(float deltaSeconds)
{
	if(isAlive)
	{
		ageSeconds += deltaSeconds;
		if(ageSeconds > lifeSpanSeconds)
		{
			isAlive = false;
			return;
		}

		Vec3 gravity = Vec3(0.f, 0.f, -9.8f);

		float curtimeRatio = ageSeconds / lifeSpanSeconds;
		float curGravityRatio;
		if(curtimeRatio < framePointRatio)
		{
			float lerpRatio = curtimeRatio / framePointRatio;
			curGravityRatio = Interpolation(gravityRatio[0], gravityRatio[1], lerpRatio);
		}
		else
		{
			float lerpRatio = (curtimeRatio - framePointRatio) / (1.f - framePointRatio);
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
	float curtimeRatio = ageSeconds / lifeSpanSeconds;
	Rgba curColor;
	if (curtimeRatio < framePointRatio)
	{
		float lerpRatio = curtimeRatio / framePointRatio;
		curColor = Lerp(color[0], color[1], lerpRatio);
	}
	else
	{
		float lerpRatio = (curtimeRatio - framePointRatio) / (1.f - framePointRatio);
		curColor = Lerp(color[1], color[2], lerpRatio);
	}

	return curColor;
}