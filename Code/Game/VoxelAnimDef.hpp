#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba.hpp"
#include <vector>

struct VoxelAnimFrame
{
	float frameRatio = 0.f;
	Vec3 pos = Vec3::ZERO;
	float scale =  0.f;
	Vec3 rotation = Vec3::ZERO;
	Rgba tint = Rgba::WHITE;
};

class VoxelAnimDef
{
public:
	VoxelAnimFrame GetFrameOnTime(float time);
	void AddFrame(VoxelAnimFrame frame) { m_frames.push_back(frame); }
	void SetAnimTime(float time) { m_animTime = time; }
	float GetAnimTime() const { return m_animTime; }

private:
	// TBD easing function
	float m_animTime = 1.f;
	std::vector<VoxelAnimFrame> m_frames;
};