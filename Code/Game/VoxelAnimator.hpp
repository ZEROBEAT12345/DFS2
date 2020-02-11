#pragma once
class VoxelAnimDef;
class VoxelAnimFrame;

enum eVoxelAnimPlayBackType
{
	PLAYBACK_ONCE,
	PLAYBACK_LOOP,
	PLAYBACK_PINGPONG,
	NUM_PLAYBACK_TYPES
};

class VoxelAnimator
{
	VoxelAnimator() {}
	~VoxelAnimator() {}

	void Update(float deltaSeconds);
	void PlayAnimation(VoxelAnimDef* anim); // Set and start anim
	void StartAnimation(); // Start current anim
	void StopAnimation(); // Pause current anim

	void SetAnimation(VoxelAnimDef* anim) { m_curAnim = anim; }
	void SetPlayBackType(eVoxelAnimPlayBackType type) { m_playbackMode = type; }
	
	bool IsPlaying() const { return m_isPlay; }
	const VoxelAnimFrame& GetCurAnimFrame();

private:
	//float m_startTime = 0.f;
	float m_curTimeCount = 0.f;
	bool m_isPlay = false;
	eVoxelAnimPlayBackType m_playbackMode = PLAYBACK_ONCE;
	VoxelAnimDef* m_curAnim = nullptr;
};