#pragma once
#include "BoneTransform.h"
#include <string>
#include <vector>

class Game;

class Skeleton
{
	struct Bone
	{
		BoneTransform mLocalBindPose;
		std::string mName;
		int mParent;
	};
public:
	Skeleton();
	~Skeleton();
	size_t GetNumBones() const { return mBones.size(); }
	const Bone& GetBone(size_t idx) const { return mBones.at(idx); }
	const std::vector<Bone> GetBones() const { return mBones; }
	const std::vector<Matrix4>& GetGlobalInvBindPoses() const { return mGlobalInvBindPoses; }

	bool Load(const WCHAR* fileName);
	static Skeleton* StaticLoad(const WCHAR* fileName, Game* pGame);
private:
	std::vector<Bone> mBones;
	std::vector<Matrix4> mGlobalInvBindPoses;

	void ComputeGlobalInvBindPose();
};