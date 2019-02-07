#pragma once
#include "component.h"
#include <unordered_map>

class Animation;
class Skeleton;
class SkinnedObj;
class JobManager;
class AnimJob;

class Character : public Component
{
public:
	Character(SkinnedObj *pObj);
    virtual ~Character() override;

    void LoadProperties(const rapidjson::Value& properties) override;

    void Update(float deltaTime) override;

	SkinnedObj* GetSkinnedObj() { return mSkinnedObj; }
	Skeleton* GetSkeleton() { return mSkeleton; }
	const Animation* GetCurrentAnim() { return mCurrentAnim; }
	float GetAnimTime() { return mAnimationTime; }

protected:
    void UpdateAnim(float deltaTime);
    bool SetAnim(const std::string& animName);

    SkinnedObj* mSkinnedObj;
    Skeleton* mSkeleton;
	std::unordered_map<std::string, const Animation*> mAnims;
    const Animation* mCurrentAnim;
    float mAnimationTime;

	// movement stuff
	float mMoveSpeed;
	float mHeading;

	JobManager* mJobManager;
	AnimJob* mJob;
};