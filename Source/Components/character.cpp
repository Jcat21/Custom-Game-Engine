#include "stdafx.h"
#include "components\character.h"
#include "animation.h"
#include "game.h"
#include "jsonUtil.h"
#include "skinnedObj.h"
#include "stringUtil.h"
#include "JobManager.h"
#include "AnimJob.h"


Character::Character(SkinnedObj *pObj)
    : Component(pObj)
    , mSkinnedObj(pObj)
    , mSkeleton(nullptr)
    , mCurrentAnim(nullptr)
    , mAnimationTime(0.0f)
	, mMoveSpeed(0.0f)
{
	Matrix4 mat = mObj->mObjectData.modelToWorld;
	mat = Matrix4::CreateRotationX(Math::PiOver2) * mat;	//mrwTODO this 90 rotation accounts for the orientation of the model :(
	Vector3 fwd = mat.GetZAxis();
	mHeading = atan2f(fwd.z, fwd.x);
	mJob = new AnimJob(this);
}

Character::~Character()
{
	delete mJob;
}

void Character::LoadProperties(const rapidjson::Value& properties)
{
    Component::LoadProperties(properties);
	std::string skeleton;
	if (GetStringFromJSON(properties, "skeleton", skeleton))
	{
		std::wstring skeletonwstr;
		StringUtil::String2WString(skeletonwstr, skeleton);
		mSkeleton = mObj->GetGame()->LoadSkeleton(skeletonwstr);
	}

	const rapidjson::Value& anims = properties["animations"];
	if (anims.IsArray())
	{
		for (rapidjson::SizeType i = 0; i < anims.Size(); ++i)
		{
			if (anims[i].IsArray() && anims[i].Size() == 2)
			{
				std::string animType = anims[i][0].GetString();
				std::string animName = anims[i][1].GetString();
				std::wstring animName_w;
				StringUtil::String2WString(animName_w, animName);
				mAnims[animType] = mObj->GetGame()->LoadAnimation(animName_w);
			}
		}
	}
}

void Character::Update(float deltaTime)
{
#if 0
	const float s_accel = 900.0f;
	const float s_moveSpeed = 300.0f;
	const float s_turnSpeed = Math::ToRadians(720.0f);

	Vector3 move = Vector3::Zero;
	Game* pGame = mObj->GetGame();
	if (pGame->IsKeyHeld(VK_UP))
		move.x += 1.0f;
	if (pGame->IsKeyHeld(VK_DOWN))
		move.x -= 1.0f;
	if (pGame->IsKeyHeld(VK_RIGHT))
		move.y += 1.0f;
	if (pGame->IsKeyHeld(VK_LEFT))
		move.y -= 1.0f;

	float throttle = move.Length();
	if (throttle > 1.0f)
		throttle = 1.0f;

	if (throttle > 0.0f)
	{	// turn to heading
		float heading = atan2f(move.y, move.x);
		float angDelta = heading - mHeading;
		if (angDelta > Math::Pi)		//mrwTODO wrap angle
			angDelta -= Math::TwoPi;
		if (angDelta < -Math::Pi)
			angDelta += Math::TwoPi;
		float angSpd = s_turnSpeed * deltaTime;
		angDelta = Math::Clamp(angDelta, -angSpd, angSpd);
		mHeading += angDelta;
		if (mHeading > Math::Pi)		//mrwTODO wrap angle
			mHeading -= Math::TwoPi;
		if (mHeading < -Math::Pi)
			mHeading += Math::TwoPi;
	}

	{	// accelerate
		float speed = throttle * s_moveSpeed;
		float spdDelta = speed - mMoveSpeed;
		float accel = s_accel * deltaTime;
		spdDelta = Math::Clamp(spdDelta, -accel, accel);
		mMoveSpeed += spdDelta;
	}

	{	// move
		Matrix4 mat = Matrix4::CreateRotationZ(mHeading);
		Vector3 vel = mMoveSpeed * mat.GetXAxis();
		Vector3 pos = mObj->mObjectData.modelToWorld.GetTranslation();
		pos += vel * deltaTime;
		mObj->mObjectData.modelToWorld = mat
			* Matrix4::CreateTranslation(pos);
	}
#endif
    if (nullptr == mCurrentAnim)
        SetAnim("idle");
    UpdateAnim(deltaTime);
}

void Character::UpdateAnim(float deltaTime)
{
    if (nullptr != mCurrentAnim)
    {
        mAnimationTime += deltaTime;
        while (mAnimationTime >= mCurrentAnim->GetLength())
            mAnimationTime -= mCurrentAnim->GetLength();
		mSkinnedObj->GetGame()->GetJobManager()->AddJob(mJob);
		/* 
        std::vector<Matrix4> pose;
        mCurrentAnim->GetGlobalPoseAtTime(pose, mSkeleton, mAnimationTime);
        const std::vector<Matrix4>& invBind = mSkeleton->GetGlobalInvBindPoses();
        for (size_t i = 0; i < pose.size(); ++i)
        {
            mSkinnedObj->mSkinnedData.matrixPalette[i] = invBind[i] * pose[i];
        }
		*/
    }
}

bool Character::SetAnim(const std::string& animName)
{
    if (mAnims.find(animName) != mAnims.end())
    {
        mCurrentAnim = mAnims[animName];
        mAnimationTime = 0.0f;
        return true;
    }
    mCurrentAnim = nullptr;
    return false;
}