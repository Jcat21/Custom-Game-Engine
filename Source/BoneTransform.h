#pragma once
#include "engineMath.h"

class BoneTransform 
{
public:
	Quaternion mRotation;
	Vector3 mTranslation;

	static BoneTransform Interpolate(const BoneTransform& a, const BoneTransform& b, float f);
public:
	BoneTransform();
	~BoneTransform();

	Matrix4 ToMatrix() const;
};