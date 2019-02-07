#include "stdafx.h"
#include "AnimJob.h"
#include "Animation.h"
#include "Components/Character.h"
#include "SkinnedObj.h"
#include <vector>

AnimJob::AnimJob(Character* pChar)
	: mCharacter(pChar)
{
}

AnimJob::~AnimJob()
{
}

void AnimJob::DoIt()
{
	std::vector<Matrix4> pose;
	mCharacter->GetCurrentAnim()->GetGlobalPoseAtTime(pose, mCharacter->GetSkeleton(), mCharacter->GetAnimTime());
	const std::vector<Matrix4>& invBind = mCharacter->GetSkeleton()->GetGlobalInvBindPoses();
	for (size_t i = 0; i < pose.size(); ++i)
	{
		mCharacter->GetSkinnedObj()->mSkinnedData.matrixPalette[i] = invBind[i] * pose[i];
	}
}
