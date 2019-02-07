#pragma once
#include "component.h"
#include "job.h"

class Animation;
class Skeleton;
class SkinnedObj;

class Character : public Component
{
public:
	Character(SkinnedObj *pObj);
    virtual ~Character() override;

    void LoadProperties(const rapidjson::Value& properties) override;

    void Update(float deltaTime) override;

protected:
    void UpdateAnim(float deltaTime);
    bool SetAnim(const std::string& animName);
    static void ProcessAnim(Character* pChar);

    SkinnedObj* mSkinnedObj;
    Skeleton* mSkeleton;
	std::unordered_map<std::string, const Animation*> mAnims;
    const Animation* mCurrentAnim;
    float mAnimationTime;
    class AnimJob : public JobManager::Job
    {
    public:
        AnimJob(Character* pCharacter);
        void DoIt() override;
        Character* mCharacter;
    };
    AnimJob mAnimJob;

	// movement stuff
	float mMoveSpeed;
	float mHeading;
};