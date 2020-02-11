#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba.hpp"
#include <vector>

struct VoxelAnimFrame
{
	float frameRatio;
	Vec3 pos;
	float scale;
	Vec3 rotation;
	Rgba tint;
};

class VoxelAnimDef
{
public:
	const VoxelAnimFrame& GetFrameOnTime(float time);
	void AddFrame(VoxelAnimFrame frame) { m_frames.push_back(frame); }
	void SetAnimTime(float time) { m_animTime = time; }
	float GetAnimTime() const { return m_animTime; }

private:
	// TBD easing function
	float m_animTime = 1.f;
	std::vector<VoxelAnimFrame> m_frames;
};