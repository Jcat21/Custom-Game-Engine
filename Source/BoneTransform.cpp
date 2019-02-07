#include "stdafx.h"
#include "BoneTransform.h"

BoneTransform BoneTransform::Interpolate(const BoneTransform & a, const BoneTransform & b, float f)
{
	BoneTransform ret;
	ret.mRotation = Lerp(a.mRotation, b.mRotation, f);
	ret.mTranslation = Lerp(a.mTranslation, b.mTranslation, f);
	return ret;
}

BoneTransform::BoneTransform()
{
	mRotation = Quaternion(0, 0, 0, 0);
	mTranslation = Vector3(0, 0, 0);
}

BoneTransform::~BoneTransform()
{
}

Matrix4 BoneTransform::ToMatrix() const
{
	return Matrix4::CreateFromQuaternion(mRotation) * Matrix4::CreateTranslation(mTranslation);
}
