#include "Game/VoxelAnimDef.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Algorithms/MathUtils.hpp"

VoxelAnimFrame VoxelAnimDef::GetFrameOnTime(float time)
{
	// Doing lerping function here
	if(time < 0.f || time > m_animTime)
	{
		ERROR_RECOVERABLE("Invalid time range!!!")
	}

	float ratio = time / m_animTime;

	// GET start frame and end frame to lerp between
	size_t startIdx, endIdx;

	startIdx = 0;
	endIdx = m_frames.size() - 1;

	if(endIdx == startIdx)
	{
		ERROR_RECOVERABLE("Not enough frame!!!")
	}

	for(int i = 0; i < m_frames.size(); i++)
	{
		float curTime = m_frames[i].frameRatio;
		float nextTime = m_frames[i + 1].frameRatio;
		if(curTime <= ratio)
		{
			startIdx = i;
		}
		else
		{
			ERROR_RECOVERABLE("No invalid start!!!")
		}

		if(nextTime >= ratio)
		{
			endIdx = i + 1;
			break;
		}
	}

	// Lerp between those 2 frames

	float curTime = m_frames[startIdx].frameRatio;
	float nextTime = m_frames[endIdx].frameRatio;

	VoxelAnimFrame newFrame;
	VoxelAnimFrame curFrame = m_frames[startIdx];
	VoxelAnimFrame nextFrame = m_frames[endIdx];
	float lerpRatio = (ratio - curTime) / (nextTime - curTime);
	
	newFrame.frameRatio = ratio;
	newFrame.pos.x = Interpolation(curFrame.pos.x, nextFrame.pos.x, lerpRatio);
	newFrame.pos.y = Interpolation(curFrame.pos.y, nextFrame.pos.y, lerpRatio);
	newFrame.pos.z = Interpolation(curFrame.pos.z, nextFrame.pos.z, lerpRatio);
	newFrame.rotation.x = Interpolation(curFrame.rotation.x, nextFrame.rotation.x, lerpRatio);
	newFrame.rotation.y = Interpolation(curFrame.rotation.y, nextFrame.rotation.y, lerpRatio);
	newFrame.rotation.z = Interpolation(curFrame.rotation.z, nextFrame.rotation.z, lerpRatio);
	newFrame.scale = Interpolation(curFrame.scale, nextFrame.scale, lerpRatio);
	newFrame.tint.r = Interpolation(curFrame.tint.r, nextFrame.tint.r, lerpRatio);
	newFrame.tint.g = Interpolation(curFrame.tint.g, nextFrame.tint.g, lerpRatio);
	newFrame.tint.b = Interpolation(curFrame.tint.b, nextFrame.tint.b, lerpRatio);
	return newFrame;
}