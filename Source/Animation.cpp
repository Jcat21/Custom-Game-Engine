#include "stdafx.h"
#include "animation.h"
#include "DbgAssert.h"
#include "game.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include <fstream>
#include <sstream>


Matrix4 BoneTransform::ToMatrix() const
{
	return Matrix4::CreateFromQuaternion(mRotation) * Matrix4::CreateTranslation(mTranslation);
}

BoneTransform BoneTransform::Interpolate(const BoneTransform& a, const BoneTransform& b, float f)
{
	BoneTransform result;
	result.mRotation = Slerp(a.mRotation, b.mRotation, f);
	result.mTranslation = Lerp(a.mTranslation, b.mTranslation, f);
	return result;
}

bool Skeleton::Load(const WCHAR* fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		return false;
	}

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();

	// Check the metadata
	if (!doc["metadata"].IsObject() ||
		str != "itpskel" ||
		ver != 1)
	{
		return false;
	}

	const rapidjson::Value& bonecount = doc["bonecount"];
	if (!bonecount.IsUint())
	{
		return false;
	}

	size_t count = bonecount.GetUint();

	DbgAssert(count <= Skeleton::MAX_SKELETON_BONES, "Skeleton exceeds maximum allowed bones.");

	mBones.reserve(count);

	const rapidjson::Value& bones = doc["bones"];
	if (!bones.IsArray())
	{
		return false;
	}

	if (bones.Size() != count)
	{
		return false;
	}

	Bone temp;

	for (rapidjson::SizeType i = 0; i < count; i++)
	{
		if (!bones[i].IsObject())
		{
			return false;
		}

		const rapidjson::Value& name = bones[i]["name"];
		temp.mName = name.GetString();

		const rapidjson::Value& parent = bones[i]["parent"];
		temp.mParent = parent.GetInt();

		const rapidjson::Value& bindpose = bones[i]["bindpose"];
		if (!bindpose.IsObject())
		{
			return false;
		}

		const rapidjson::Value& rot = bindpose["rot"];
		const rapidjson::Value& trans = bindpose["trans"];

		if (!rot.IsArray() || !trans.IsArray())
		{
			return false;
		}

		temp.mLocalBindPose.mRotation.x = rot[0].GetDouble();
		temp.mLocalBindPose.mRotation.y = rot[1].GetDouble();
		temp.mLocalBindPose.mRotation.z = rot[2].GetDouble();
		temp.mLocalBindPose.mRotation.w = rot[3].GetDouble();

		temp.mLocalBindPose.mTranslation.x = trans[0].GetDouble();
		temp.mLocalBindPose.mTranslation.y = trans[1].GetDouble();
		temp.mLocalBindPose.mTranslation.z = trans[2].GetDouble();

		mBones.push_back(temp);
	}

	// Now that we have the bones
	ComputeGlobalInvBindPose();

	return true;
}

Skeleton* Skeleton::StaticLoad(const WCHAR* fileName, Game* pGame)
{
	Skeleton *pSkeleton = new Skeleton();
    if (false == pSkeleton->Load(fileName))
    {
        delete pSkeleton;
        return nullptr;
    }
    return pSkeleton;
}

void Skeleton::ComputeGlobalInvBindPose()
{
	mGlobalInvBindPoses.resize(mBones.size());
	for (size_t i = 0; i < mBones.size(); ++i)
	{
		mGlobalInvBindPoses[i] = mBones[i].mLocalBindPose.ToMatrix();
		if (mBones[i].mParent >= 0)
			mGlobalInvBindPoses[i] = mGlobalInvBindPoses[i] * mGlobalInvBindPoses[mBones[i].mParent];
	}
	for (size_t i = 0; i < mBones.size(); ++i)
	{
		mGlobalInvBindPoses[i].Invert();
	}
}

Animation::Animation(Game* game)
{
}

bool Animation::Load(const WCHAR* fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		return false;
	}

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();

	// Check the metadata
	if (!doc["metadata"].IsObject() ||
		str != "itpanim" ||
		ver != 2)
	{
		return false;
	}

	const rapidjson::Value& sequence = doc["sequence"];
	if (!sequence.IsObject())
	{
		return false;
	}

	const rapidjson::Value& frames = sequence["frames"];
	const rapidjson::Value& length = sequence["length"];
	const rapidjson::Value& bonecount = sequence["bonecount"];

	if (!frames.IsUint() || !length.IsDouble() || !bonecount.IsUint())
	{
		return false;
	}

	mNumFrames = frames.GetUint();
	mLength = length.GetDouble();
	mNumBones = bonecount.GetUint();

	mTracks.resize(mNumBones);

	const rapidjson::Value& tracks = sequence["tracks"];

	if (!tracks.IsArray())
	{
		return false;
	}

	for (rapidjson::SizeType i = 0; i < tracks.Size(); i++)
	{
		if (!tracks[i].IsObject())
		{
			return false;
		}

		size_t boneIndex = tracks[i]["bone"].GetUint();

		const rapidjson::Value& transforms = tracks[i]["transforms"];
		if (!transforms.IsArray())
		{
			return false;
		}

		BoneTransform temp;

		if (transforms.Size() != mNumFrames)
		{
			return false;
		}

		for (rapidjson::SizeType j = 0; j < transforms.Size(); j++)
		{
			const rapidjson::Value& rot = transforms[j]["rot"];
			const rapidjson::Value& trans = transforms[j]["trans"];

			if (!rot.IsArray() || !trans.IsArray())
			{
				return false;
			}

			temp.mRotation.x = rot[0].GetDouble();
			temp.mRotation.y = rot[1].GetDouble();
			temp.mRotation.z = rot[2].GetDouble();
			temp.mRotation.w = rot[3].GetDouble();

			temp.mTranslation.x = trans[0].GetDouble();
			temp.mTranslation.y = trans[1].GetDouble();
			temp.mTranslation.z = trans[2].GetDouble();

			mTracks[boneIndex].emplace_back(temp);
		}
	}

	return true;
}

Animation* Animation::StaticLoad(const WCHAR* fileName, Game* pGame)
{
	Animation *pAnimation = new Animation(pGame);
	if (false == pAnimation->Load(fileName))
	{
		delete pAnimation;
		return nullptr;
	}
	return pAnimation;
}

void Animation::GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const Skeleton *inSkeleton, float inTime) const
{
    int fromKey = 0;
    int toKey = 0;
    float lerp = 0.0f;
    if (inTime > mLength)
    {
        fromKey = mNumFrames - 1;
        toKey = fromKey;
    }
    else if (inTime > 0.0f)
    {
        float timePerFrame = mLength / (mNumFrames - 1);
        float frac = inTime / timePerFrame;
        fromKey = (int)frac;
        toKey = fromKey + 1;
        lerp = frac - fromKey;
    }

	size_t numBones = inSkeleton->GetNumBones();
    outPoses.resize(numBones);
    for (size_t i = 0; i < numBones; ++i)
    {
        const Skeleton::Bone& bone = inSkeleton->GetBone(i);
        if (mTracks[i].size() > 0)
        {
            BoneTransform xform = BoneTransform::Interpolate(mTracks[i][fromKey], mTracks[i][toKey], lerp);
            outPoses[i] = xform.ToMatrix();
        }
        else
            outPoses[i] = Matrix4::Identity;
        if (bone.mParent >= 0)
            outPoses[i] = outPoses[i] * outPoses[bone.mParent];
    }
}
