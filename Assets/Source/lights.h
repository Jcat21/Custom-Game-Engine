#pragma once
#include "engineMath.h"

const size_t MAX_POINT_LIGHTS = 8;

struct PointLightData
{
	Vector3 mDiffuseColor;
	float pad0;
    Vector3 mSpecularColor;
	float pad1;
    Vector3 mPosition;
	float mSpecularPower;
    float mInnerRadius;
	float mOuterRadius;
	uint32_t mIsEnabled;
	float pad2;
};
