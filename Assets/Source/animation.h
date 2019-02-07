#pragma once
#include <vector>
#include "engineMath.h"

class Game;

class BoneTransform
{
public:
	// For now, just make this data public
	Quaternion mRotation;
	Vector3 mTranslation;

	Matrix4 ToMatrix() const;
    static BoneTransform Interpolate(const BoneTransform& a, const BoneTransform& b, float f);
};

class Skeleton
{
public:
	static const uint32_t MAX_SKELETON_BONES = 96;

	struct Bone
	{
		BoneTransform mLocalBindPose;
		std::string mName;
		int mParent;
	};

	bool Load(const WCHAR* fileName);
	static Skeleton* StaticLoad(const WCHAR* fileName, Game* pGame);

	size_t GetNumBones() const { return mBones.size(); }
	const Bone& GetBone(size_t idx) const { return mBones[idx]; }
	const std::vector<Bone>& GetBones() const { return mBones; }

	const std::vector<Matrix4>& GetGlobalInvBindPoses() const { return mGlobalInvBindPoses; }

private:
	void ComputeGlobalInvBindPose();

	std::vector<Bone> mBones;
	std::vector<Matrix4> mGlobalInvBindPoses;
};

class Animation
{
public:
	Animation(Game* game);

	bool Load(const WCHAR* fileName);
	static Animation* StaticLoad(const WCHAR* fileName, Game* pGame);

	uint32_t GetNumBones() const { return mNumBones; }
	uint32_t GetNumFrames() const { return mNumFrames; }
	float GetLength() const { return mLength; }

	// Fills the provided matrix with the global (current) pose matrices for each
	// bone at the specified time in the animation. It is expected that the time
	// is >= 0.0f and <= mLength
	void GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const Skeleton *inSkeleton, float inTime) const;

private:
	// Number of bones for the animation
	uint32_t mNumBones;

	// Number of frames in the animation
	uint32_t mNumFrames;

	// Length of the animation in seconds
	float mLength;

	// Transform information for each frame on the track
	// Each index in the outer vector is a bone, inner vector
	// is a frame
	std::vector<std::vector<BoneTransform>> mTracks;
};