#include "Game/VoxelAnimator.hpp"
#include "Game/VoxelAnimDef.hpp"

void VoxelAnimator::Update(float deltaSeconds)
{
	if (!m_isPlay)
		return;

	m_curTimeCount += deltaSeconds;
	float animTime = m_curAnim->GetAnimTime();

	switch (m_playbackMode)
	{
	case PLAYBACK_ONCE:
		m_isPlay = false;
		m_curTimeCount = 0.f;
		break;
	case PLAYBACK_LOOP:
		while (m_curTimeCount > animTime)
		{
			m_curTimeCount -= animTime;
		}
		break;
	case PLAYBACK_PINGPONG:
		// TBD;
		break;
	default:
		break;
	}

	if(m_playbackMode == PLAYBACK_LOOP)
	{
		while (m_curTimeCount > animTime)
		{
			m_curTimeCount -= animTime;
		}
	}
}

void VoxelAnimator::PlayAnimation(VoxelAnimDef* anim)
{
	SetAnimation(anim);
	StartAnimation();
}

void VoxelAnimator::StartAnimation() // Start current anim
{
	m_isPlay = true;
	m_curTimeCount = 0.f;
}

void VoxelAnimator::StopAnimation() // Pause current anim
{
	m_isPlay = false;
	m_curTimeCount = 0.f;
}

const VoxelAnimFrame& VoxelAnimator::GetCurAnimFrame()
{
	return m_curAnim->GetFrameOnTime(m_curTimeCount);
}