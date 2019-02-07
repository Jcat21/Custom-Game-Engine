#include "stdafx.h"
#include "Skeleton.h"
#include "stringUtil.h"
#include "game.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include <fstream>
#include <sstream>

Skeleton::Skeleton()
{
}

Skeleton::~Skeleton()
{
}

bool Skeleton::Load(const WCHAR * fileName)
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

	const rapidjson::Value& bones = doc["bones"];
	if (!bones.IsArray() || bones.Size() < 1)
	{
		return false;
	}

	auto numBones = doc["bonecount"].GetUint();
	for (rapidjson::SizeType i = 0; i < numBones; i++)
	{
		Bone pBone;
		std::string boneName = bones[i]["name"].GetString();
		int parent = bones[i]["parent"].GetInt();
		Quaternion rot;
		rot.x = bones[i]["bindpose"]["rot"][0].GetDouble();
		rot.y = bones[i]["bindpose"]["rot"][1].GetDouble();
		rot.z = bones[i]["bindpose"]["rot"][2].GetDouble();
		rot.w = bones[i]["bindpose"]["rot"][3].GetDouble();
		Vector3 trans;
		trans.x = bones[i]["bindpose"]["trans"][0].GetDouble();
		trans.y = bones[i]["bindpose"]["trans"][1].GetDouble();
		trans.z = bones[i]["bindpose"]["trans"][2].GetDouble();
		pBone.mLocalBindPose.mRotation = rot;
		pBone.mLocalBindPose.mTranslation = trans;
		pBone.mName = boneName;
		pBone.mParent = parent;
		mBones.push_back(pBone);
		mGlobalInvBindPoses.push_back(pBone.mLocalBindPose.ToMatrix());
	}
	ComputeGlobalInvBindPose();
	return true;
}

Skeleton * Skeleton::StaticLoad(const WCHAR * fileName, Game * pGame)
{
	Skeleton* skeleton = new Skeleton();
	if (false == skeleton->Load(fileName))
	{
		delete skeleton;
		return nullptr;
	}
	return skeleton;
}

void Skeleton::ComputeGlobalInvBindPose()
{
	Matrix4 newBindPose;

	for (int i = 0; i < mBones.size(); ++i)
	{
		newBindPose = mBones.at(i).mLocalBindPose.ToMatrix();
		for (int j = mBones.at(i).mParent; j >= 0; j = mBones.at(j).mParent)
		{
			newBindPose *= mBones.at(j).mLocalBindPose.ToMatrix();
		}
		mGlobalInvBindPoses.at(i) = newBindPose;
	}
	for (int i = 0; i < mBones.size(); ++i)
	{
		mGlobalInvBindPoses.at(i).Invert();
	}
}
